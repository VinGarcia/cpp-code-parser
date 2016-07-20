#include <cstring>

#include "./generators.h"

class Range : public Iterator {
  long from, to, step, i;

 public:
  Range(long from, long to, long step) : from(from), to(to), step(step), i(from) {
    this->type = IT;
  }

  packToken* next() {
    long value = i;
    i += step;
    if (value < to) {
      return new packToken((double)value);
    } else {
      i = from;
      return NULL;
    }
  }

  void reset() {
    i=from;
  }

  virtual TokenBase* clone() const {
    return new Range(static_cast<const Range&>(*this));
  }
};

const char* range_args[] = {"from", "to", "step"};
packToken default_range(const Scope* scope) {
  long to, step, from;

  packToken* p_from = scope->find("from");
  packToken* p_to = scope->find("to");
  packToken* p_step = scope->find("step");

  if ((*p_from)->type == NUM) {
    from = p_from->asDouble();
  } else if ((*p_from)->type == NONE) {
    throw std::invalid_argument("range() expects at least the first argument!");
  } else {
    throw std::invalid_argument("range() expects only numbers!");
  }

  if ((*p_to)->type == NONE) {
    to = from;
    from = 0;
    step = 1;
  } else if ((*p_to)->type == NUM) {
    to = p_to->asDouble();

    if ((*p_step)->type == NUM) {
      step = p_step->asDouble();
    } else if ((*p_step)->type == NONE) {
      step = 1;
    } else {
      throw std::invalid_argument("range() expects only numbers!");
    }
  } else {
    throw std::invalid_argument("range() expects only numbers!");
  }

  return Range(from, to, step);
}

struct Iterator::Startup {
  Startup() {
    TokenMap_t& global = Scope::default_global();
    global["range"] = Function(default_range, 3, range_args);
  }
} iterator_startup;

// UserFunction::buildNames(strList args) {

// }

UserFunction::UserFunction(strList args, BlockStatement body, std::string name) :
                           Function(0, args.size(), 0, name), body(body) {
  // Build the arg names array:
  uint i = 0;
  this->arg_names = new const char*[this->nargs];
  for (std::string& arg : args) {
    char* aux = new char[arg.size()+1];
    strcpy(aux, arg.c_str());
    arg_names[i] = aux;
    ++i;
  }
}

UserFunction::UserFunction(const UserFunction& other) :
                           Function(0, other.nargs, 0, other.name), body(other.body) {
  // Build the arg names array:
  this->arg_names = new const char*[nargs];
  for (uint i = 0; i < nargs; ++i) {
    uint len = strlen(other.arg_names[i]);
    char* aux = new char[len+1];
    strcpy(aux, other.arg_names[i]);
    arg_names[i] = aux;
    ++i;
  }
}

void UserFunction::cleanArgs() {
  for(uint i = 0; i < this->nargs; ++i) {
    delete arg_names[i];
  }
  delete[] arg_names;
}

packToken UserFunction::exec(const Scope& scope) {
  body.exec(scope);
  return packToken::None;
}
