#include "fa.h"
#include "faprint.h"
#include "tgroups.h"
#include <iostream.h>
#include <strstream.h>
#include <stdio.h>
#include <ctype.h>

struct scanner_data
{
  int       min_body_size;
  int       max_rest_size;

  void render_static_data(const FA& dfa, TGroupMgr& tmg, ostream& out);
  void render_scanner(const FA& dfa, ostream& out);
  void render_symbol_cases(const State* state, const TGroupMgr& tgm, ostream& out);
};


inline bool needs_escape(int c)
{
  return c == '\'' || c == '\\';
}

void render_char(int charr, ostream& out)
{
  if (isprint(charr))
    out << (needs_escape(charr) ? "'\\" : "'") << (char)charr << "\'";
  else
    out << (int)charr;
}

void render_state_transition(const State* from, const State* to, ostream& out)
{
  if (from != to) {
    out << " s = " << to->id();
/*
    if (!from->final_codes().empty()
        && to->final_codes().empty()) {
      out << "; b.mark(-1); a = "
          << (*from->final_codes().begin()).value();
    }
*/
  }
  out << ";";
}

void  render_goto_action(const State* state, ostream& out)
{
  out << "goto _A";
  if (!state->final_codes().empty()) {
    out << "_" << (*state->final_codes().begin()).value();
  }
  out << ";\n";
}


void  scanner_data::render_static_data(const FA& dfa, TGroupMgr& tgm, ostream& out)
{
  if (min_body_size == 0) {
    return;
  }

  tgm.adjoin(dfa, min_body_size);

  int arr_size = CHAR_COUNT;
  const TGroup::vector& groups = tgm.groups();
  for (int i=0; i < groups.size(); ++i) {
    TGroup* group = groups[i];
    int rest = CHAR_COUNT - (group->mask().size() + group->body().size());
    if (rest <= max_rest_size) {
      ostrstream buf;
      if (rest == 0) {
        buf << "true";
      }
      else {
        buf << "(";
        bool first = true;
        for (Char c = CHAR_FIRST; c <= CHAR_LAST; ++c) {
          if (!const_cast<CharSet&>(group->body()).find(c)
           && !const_cast<CharSet&>(group->mask()).find(c)) {
            if (first)
              first = false;
            else
              buf << " && ";
            buf << "c != ";
            render_char(c, buf);
          }
        }
        buf << ")";
      }
      buf << ends;
      char* cond = buf.str();
      group->set_text(cond);
      out << "// " << cond << "\n";
      delete[] cond;
      continue;
    }
    char cond[32];
    sprintf(cond, "isgroup%d", i);
    out << "// ";
    PrintGroup(*group, out);
    out << "\n"
           "static const char " << cond << "[" << arr_size << "] = {";
    strcat(cond, "[c]");
    group->set_text(cond);
    Char prev = -1;
    CharSet::const_iterator ci = group->body().begin();
    CharSet::const_iterator ce = group->body().end();
    while (true) {
      Char curr = *ci;
      for (Char c = prev + 1; c < curr; ++c) {
        out << "0,";
      }
      prev = curr;
      if (++ci == ce) {
        out << "1};\n";
        break;
      }
      out << "1,";
    }
  }

  out << "static int action[" << dfa.n_states() << "] = {";
  for (int s=0; s < dfa.n_states(); ++s) {
    if (s) {
      out << ",";
    }
    const State::FinalCodes& final_codes = dfa.state(s)->final_codes();
    if (final_codes.empty()) {
      out << "0";
    }
    else {
      out << (*final_codes.begin()).value();
    }
  }
  out << "};\n";
}


void  scanner_data::render_symbol_cases(const State* state, const TGroupMgr& tgm, ostream& out)
{
  const char* tab = "      ";
  State::set grouped(!OWNS_DATA);
  // first render non-grouped symbols:
  for (State::TransitionsIter ti  = state->transitions().begin();
                              ti != state->transitions().end();
                            ++ti) {
    Transition t = *ti;
    State* s = t.state;
    const TGroup* group = tgm.lookup_group(state->id(), s->id());
    if (!group) {
      out << tab << "case ";
      render_char(t.charr, out);
      out << ":";
      render_state_transition(state, t.state, out);
      out << " break;\n";
    }
    else {
      grouped.insert_unique(s);
    }
  }
  out << tab << "default:\n" << tab << "  ";
  for (State::set::iterator si  = grouped.begin(), se = grouped.end();
                            si != se;
                          ++si) {
    State* s = *si;
    const TGroup* group = tgm.lookup_group(state->id(), s->id());
    if (0 == strcmp(group->text(), "true")) {
      render_state_transition(state, s, out);
      out << "\n";
      return;
    }
    out << "if (" << group->text() << ")";
    render_state_transition(state, s, out);
    out << "\n" << tab << "  else ";
  }
  render_goto_action(state, out);
}


void  scanner_data::render_scanner(const FA& dfa, ostream& out)
{
  out << "#include \"scanner.h\"\n\n";

  TGroupMgr tgm;

  render_static_data(dfa, tgm, out);

  out << "\n"
         "int scanner::next_token()\n"
         "{\n"
         "  scan_buffer& b = *buffer;\n"
         "  b.flush_to_mark();\n"
         "  int s = 0;\n"
         "  int a = 0;\n"
         "  int c;\n"
         "  while (1) {\n"
         "    if (action[s]) {\n"
         "      a = action[s];\n"
         "      b.mark();\n"
         "    }\n"
         "    if ((c = b.get()) < 0) {\n"
         "      break;\n"
         "    }\n"
         "    switch (s) {\n";
  State::FinalCodes final_codes;
  for (int s=0; s < dfa.n_states(); ++s) {
    out << "    case " << s << ":\n";
    const State* state = dfa.state(s);
    if (!state->final_codes().empty()) {
      final_codes.insert_unique(*state->final_codes().begin());
    }
    const State::Transitions& transitions = state->transitions();
    if (transitions.empty()) {
      out << "      ";
      render_goto_action(state, out);
    }
    else {
      out << "      switch (c) {\n";
      render_symbol_cases(state, tgm, out);
      out << "      }\n"
          << "      break;\n";
    }
  }
  out << "    }\n"
         "  }\n";

  out << " _A:\n"
         "  if (a == 0) {\n"
         "    b.mark();\n"
         "    return (0 <= c ? 0 : -1);\n"
         "  }\n"
         "  else {\n"
         "    b.reset_to_mark();\n"
         "  }\n"
         "  switch (a) {\n";

  for (State::FinalCodesIter fi  = final_codes.begin();
                             fi != final_codes.end();
                           ++fi) {
    const FinalCode& fc = *fi;
    out << "      _A_" << fc.value() << ": b.reset_to_mark();\n"
           "    case " << fc.value() << ": return " << fc.value() << ";\n";
  }
  out << "  }\n"
         "  return -1;\n"
         "}\n";
}


void  make_scanner(const FA& fa, ostream& out) {
  scanner_data pd;

  pd.min_body_size = 5;
  pd.max_rest_size = 4;

  pd.render_scanner(fa, out);
}
