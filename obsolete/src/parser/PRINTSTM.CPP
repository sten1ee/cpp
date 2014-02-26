#include "parser/asprint.h"


int   AbstractSyntaxPrinter::nTabs = 0;
bool  AbstractSyntaxPrinter::DisableNewLine = false;


void  AbstractSyntaxPrinter::NewLine(ostream& os)
{
  if (DisableNewLine) {
    return;
  }
  os << endl;
  for (int i=nTabs; 0 < i--; ) {
    os << "  ";
  }
}


void  AbstractSyntaxPrinter::PrintDims(int ndims, ostream& os)
{
  while (0 < ndims--)
    os << "[]";
}


void  AbstractSyntaxPrinter::PrintParamList(VarDecl::Vector& param, ostream& os)
{
  os << "(";
  for (size_t i=0; i < param.size(); i++) {
    if (i) {
      os << ", ";
    }
    PrintExp(param[i]->typeExp, os);
    os << " " << param[i]->sym;
    PrintDims(param[i]->xdims, os);
  }
  os << ")";
}


void  AbstractSyntaxPrinter::PrintStm(Stm* stm, ostream& os)
{
  NewLine(os);
  switch (stm->st) {
    case  ST_EMPTY: {
      os << ";";
      break;
    }
    case  ST_IF: {
      IfStm* ifStm = SURE_CAST(stm, IfStm);
      os << "if (";
      PrintExp(ifStm->condExp, os);
      os << ")";
      nTabs++;
      PrintStm(ifStm->thenStm, os);
      nTabs--;
      if (ifStm->elseStm) {
        NewLine(os);
        os << "else";
        nTabs++;
        PrintStm(ifStm->elseStm, os);
        nTabs--;
      }
      break;
    }
    case  ST_WHILE: {
      WhileStm* whileStm = SURE_CAST(stm, WhileStm);
      os << "while (";
      PrintExp(whileStm->condExp, os);
      os << ")";
      nTabs++;
      PrintStm(whileStm->bodyStm, os);
      nTabs--;
      break;
    }
    case  ST_DO_WHILE: {
      DoWhileStm* doWhileStm = SURE_CAST(stm, DoWhileStm);
      os << "do";
      nTabs++;
      PrintStm(doWhileStm->bodyStm, os);
      nTabs--;
      NewLine(os);
      os << "while (";
      PrintExp(doWhileStm->condExp, os);
      os << ");";
      break;
    }
    case  ST_FOR: {
      ForStm* forStm = SURE_CAST(stm, ForStm);
      os << "for (";
      if (forStm->initStm) {
        DisableNewLine = true;
        PrintStm(forStm->initStm, os);
        DisableNewLine = false;
      }
      else {
        os << ';';
      }
      os << ' ';
      if (forStm->condExp) {
        PrintExp(forStm->condExp, os);
      }
      os << "; ";
      if (forStm->alterStm) {
        CompoStm* compoStm = SAFE_CAST(forStm->alterStm, CompoStm);
        if (compoStm) {
          for (size_t i=0; i < compoStm->stm.size(); i++) {
            Exp* alterExp = SURE_CAST(compoStm->stm[i], ExpStm)->exp;
            if (i) {
              os << ", ";
            }
            PrintExp(alterExp, os);
          }
        }
        else {
          PrintExp(SURE_CAST(forStm->alterStm, ExpStm)->exp, os);
        }
      }
      os << ")";

      nTabs++;
      PrintStm(forStm->bodyStm, os);
      nTabs--;
      break;
    }
    case  ST_SYNCHRO: {
      SynchroStm* synchroStm = SURE_CAST(stm, SynchroStm);
      os << "synchronized (";
      PrintExp(synchroStm->synchroExp, os);
      os << ")";
      PrintBodyStm(synchroStm->bodyStm, os);
      break;
    }
    case ST_RETURN: {
      ReturnStm* returnStm = SURE_CAST(stm, ReturnStm);
      os << "return";
      if (returnStm->returnedExp) {
        os << " ";
        PrintExp(returnStm->returnedExp, os);
      }
      os << ";";
      break;
    }
    case ST_BREAK: {
      BreakStm* breakStm = SURE_CAST(stm, BreakStm);
      os << "break";
      if (breakStm->labelSym) {
        os << " " << breakStm->labelSym;
      }
      os << ";";
      break;
    }
    case ST_CONTINUE: {
      ContinueStm* continueStm = SURE_CAST(stm, ContinueStm);
      os << "continue";
      if (continueStm->labelSym) {
        os << " " << continueStm->labelSym;
      }
      os << ";";
      break;
    }
    case  ST_LABEL: {
      LabelStm* labelStm = SURE_CAST(stm, LabelStm);
      nTabs--;
      while (true) {
        NewLine(os);
        os << labelStm->labelSym << ":";
        if (SAFE_CAST(labelStm->labeledStm, LabelStm)) {
          labelStm = SURE_CAST(labelStm->labeledStm, LabelStm);
        }
        else {
          break;
        }
      }
      nTabs++;
      PrintStm(labelStm->labeledStm, os);
      break;
    }
    case  ST_DEFAULT: {
      nTabs--;
      NewLine(os);
      nTabs++;
      os << "default:";
      break;
    }
    case  ST_CASE: {
      nTabs--;
      NewLine(os);
      nTabs++;
      CaseStm* caseStm = SURE_CAST(stm, CaseStm);
      os << "case ";
      PrintExp(caseStm->casedExp, os);
      os << ":";
      break;
    }
    case  ST_SWITCH: {
      SwitchStm* switchStm = SURE_CAST(stm, SwitchStm);
      os << "switch (";
      PrintExp(switchStm->switchedExp, os);
      os << ")";
      PrintBodyStm(switchStm->bodyStm, os);
      break;
    }
    case  ST_THROW: {
      ThrowStm* throwStm = SURE_CAST(stm, ThrowStm);
      os << "throw ";
      PrintExp(throwStm->thrownExp, os);
      os << ";";
      break;
    }
    case  ST_EXP: {
      ExpStm* expStm = SURE_CAST(stm, ExpStm);
      PrintExp(expStm->exp, os);
      os << ";";
      break;
    }
    case  ST_TRY: {
      TryStm* tryStm = SURE_CAST(stm, TryStm);
      os << "try";
      PrintBodyStm(tryStm->triedStm, os);
      CatchClause::Vector& catchClause = tryStm->catchClause;
      for (size_t i=0; i < catchClause.size(); i++) {
        NewLine(os);
        os << "catch ";
        VarDecl::Vector tmp(!OWNS_DATA);
        tmp.push(catchClause[i]->param);
        PrintParamList(tmp, os);
        PrintBodyStm(catchClause[i]->stm, os);
      }
      if (tryStm->finallyStm) {
        NewLine(os);
        os << "finally";
        PrintBodyStm(tryStm->finallyStm, os);
      }
      break;
    }
    case  ST_DECL: {
      DeclStm* declStm = SURE_CAST(stm, DeclStm);
      PrintExp(declStm->typeExp, os);
      VarDecl::Vector& var = declStm->varDecl;
      if (!var.isEmpty()) {
        size_t i=0;
        while (true) {
          os << " " << var[i]->sym;
          PrintDims(var[i]->xdims, os);
          if (var[i]->initExp) {
            os << " = ";
            PrintExp(var[i]->initExp, os);
          }
          if (++i == var.size()) {
            os << ";";
            break;
          }
          else {
            os << ",";
          }
        }
      }
      break;
    }
    case  ST_COMPO: {
      CompoStm* compoStm = SURE_CAST(stm, CompoStm);
      os << "{";
      nTabs++;
      for (size_t i=0; i < compoStm->stm.size(); i++) {
        PrintStm(compoStm->stm[i], os);
      }
      nTabs--;
      NewLine(os);
      os << "}";
      break;
    }
    default: {
      PARSER_ASSERT(0);
    }
  }
}


void  AbstractSyntaxPrinter::PrintModifs(int modifs, ostream& os)
{
  if (modifs & M_PUBLIC        ) os << "public ";
  if (modifs & M_PRIVATE       ) os << "private ";
  if (modifs & M_PROTECTED     ) os << "protected ";
  if (modifs & M_STATIC        ) os << "static ";
  if (modifs & M_FINAL         ) os << "final ";
  if (modifs & M_SYNCHRONIZED  ) os << "synchronized ";
  if (modifs & M_THREADSAFE    ) os << "threadsafe ";
  if (modifs & M_TRANSIENT     ) os << "transient ";
  if (modifs & M_NATIVE        ) os << "native ";
  if (modifs & M_ABSTRACT      ) os << "abstract ";
}


void  AbstractSyntaxPrinter::PrintDecl(Decl* decl, ostream& os)
{
  NewLine(os);
  if (decl->modifs) {
    PrintModifs(decl->modifs, os);
    os << "  ";
  }

  switch (decl->dt) {
    case DT_INSTVAR:
    case DT_LOCALVAR: {
      VarDecl* var = SURE_CAST(decl, VarDecl);
      PrintExp(var->typeExp, os);
      os << "  ";
      os << var->sym;
      PrintDims(var->xdims, os);
      if (var->initExp) {
        os << " = ";
        PrintExp(var->initExp, os);
      }
      os << ";";
      break;
    }

    case DT_METHOD:
    case DT_CTOR: {
      MethodDecl* method = SURE_CAST(decl, MethodDecl);
      if (method->typeExp) {
        PrintExp(method->typeExp, os);
        os << "  ";
      }
      os << method->sym;
      PrintParamList(method->param, os);
      PrintDims(method->xdims, os);
      if (!method->throwsClass.isEmpty()) {
        os << " throws ";
        for (int i=0; i < method->throwsClass.size(); i++) {
          if (i) {
            os << ", ";
          }
          PrintExp(method->throwsClass[i], os);
        }
      }
      if (method->bodyStm) {
        PrintBodyStm(method->bodyStm, os);
        break;
      }
      else {
        os << ";";
      }
      break;
    }

    case DT_STATICBLOCK: {
      PrintBodyStm(SURE_CAST(decl, StaticBlock)->bodyStm, os);
      break;
    }
    default: {
      PARSER_ASSERT(!"case");
    }
  }
}


void  AbstractSyntaxPrinter::PrintBodyStm(Stm* bodyStm, ostream& os)
{
  nTabs++;
  if (SAFE_CAST(bodyStm, CompoStm)) {
    PrintStm(bodyStm, os);
  }
  else if (SAFE_CAST(bodyStm, EmptyStm)) {
    os << " {}";
  }
  else {
    NewLine(os);
    os << "{";
    nTabs++;
    PrintStm(bodyStm, os);
    nTabs--;
    NewLine(os);
    os << "}";
  }
  nTabs--;
}


void  AbstractSyntaxPrinter::PrintClassDecl(ClassDecl* classDecl, ostream& os)
{
  NewLine(os);
  NewLine(os);

  PrintModifs(classDecl->modifs, os);
  if (classDecl->modifs & M_INTERFACE) {
    os << "interface " << classDecl->sym;
  }
  else {
    os << "class " << classDecl->sym;
    if (classDecl->baseClassId) {
      os << " extends ";
      PrintExp(classDecl->baseClassId, os);
    }
  }

  if (!classDecl->interfaceId.isEmpty()) {
    os << " implements ";
    for (size_t i=0; i < classDecl->interfaceId.size(); i++) {
      if (i) {
        os << ", ";
      }
      PrintExp(classDecl->interfaceId[i], os);
    }
  }
  NewLine(os);
  os << "{";
  nTabs++;
  for (size_t i=0; i < classDecl->decl.size(); i++) {
    NewLine(os);
    PrintDecl(classDecl->decl[i], os);
  }
  nTabs--;
  NewLine(os);
  os << "}";
}


void  AbstractSyntaxPrinter::PrintModule(Module* module, ostream& os)
{
  nTabs = 0;
  if (module->packageId) {
    os << "package ";
    PrintExp(module->packageId, os);
    os << ";";
  }
  size_t i;
  for (i = 0; i < module->import.size(); i++) {
    NewLine(os);
    os << "import ";
    PrintExp(module->import[i]->id, os);
    if (module->import[i]->bPackage) {
      os << ".*";
    }
    os << ";";
  }
  for (i = 0; i < module->classDecl.size(); i++) {
    PrintClassDecl(module->classDecl[i], os);
  }
}

