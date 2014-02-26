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
  char*     prototype;
  char*     func_getc;
  char*     char_eoi;

  void render_static_code(const TGroupMgr& tgm, ostream& out);
  void render_scanner(const FA& dfa, ostream& out);
  void render_symbol_cases(const State* state, const TGroupMgr& tgm, ostream& out);
};


inline bool needs_escape(int c)
{
  return c == '\'' || c == '\\';
}

inline void render_char(int charr, ostream& out)
{
  if (isprint(charr))
    out << (needs_escape(charr) ? "'\\" : "'") << (char)charr << "\'";
  else
    out << (int)charr;
}


void  scanner_data::render_static_code(const TGroupMgr& tgm, ostream& out)
{
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
      out << "// " << cond;
      delete[] cond;
      continue;
    }
    char cond[32];
    sprintf(cond, "isgroup%d", i);
    out << "// ";
    PrintGroup(*group, out);
    out << "\n";
    out << "static const char " << cond << "[" << arr_size << "] = {";
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
      out << ": s = " << t.state->id() << "; break;\n";
    }
    else {
      grouped.insert_unique(s);
    }
  }
  if (!state->final_codes().empty()) {
    out << tab << "case " << char_eoi << ": return "
        << (*state->final_codes().begin()).value()
        << ";\n";
  }
  out << tab << "default:\n" << tab << "  ";
  for (State::set::iterator si  = grouped.begin(), se = grouped.end();
                            si != se;
                          ++si) {
    State* s = *si;
    const TGroup* group = tgm.lookup_group(state->id(), s->id());
    if (0 == strcmp(group->text(), "true")) {
      if (state != s) {
        out << "s = " << s->id();
      }
      out << ";\n";
      return;
    }
    out << "if (" << group->text() << ")";
    if (state != s)
      out << " s = " << s->id();
    out << ";\n" << tab << "  else ";
  }
  out << "return 0;\n";
}


void  scanner_data::render_scanner(const FA& dfa, ostream& out)
{
  TGroupMgr tgm;

  if (min_body_size > 1) {
    tgm.adjoin(dfa, min_body_size);
    render_static_code(tgm, out);
    out << "\n";
  }

  out << prototype << "\n"
      << "{\n"
      << "  int s = 0;\n"
      << "  while (1) {\n"
      << "    int c = " << func_getc << ";\n"
      << "    switch (s) {\n";
  for (int s=0; s < dfa.n_states(); ++s) {
    out << "    case " << s << ":\n";
    const State* state = dfa.state(s);
    const State::Transitions& transitions = state->transitions();
    if (transitions.empty()) {
      if (!state->final_codes().empty()) {
        out << "      return c == " << char_eoi << " ? "
            << (*state->final_codes().begin()).value()
            << " : 0;\n";
      }
      else {
        out << "      return 0;";
      }
      continue;
    }

    out << "      switch (c) {\n";
    render_symbol_cases(state, tgm, out);
    out << "      }\n"
        << "      break;\n";
  }
  out << "    }\n"
      << "  }\n"
      << "}\n";
}


void  make_scanner(const FA& fa, ostream& out) {
  scanner_data pd;

  pd.min_body_size = 5;
  pd.max_rest_size = 4;
  pd.prototype = "int token(const char* str)";
  pd.func_getc = "*str++";
  pd.char_eoi  = "0";

  pd.render_scanner(fa, out);
}