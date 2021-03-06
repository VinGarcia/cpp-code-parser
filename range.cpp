#include <cstring>

#include "./range.h"

class Range::RangeIterator : public Iterator {
  int64_t from, to, step, i;
  packToken last;

 public:
  RangeIterator(int64_t from, int64_t to, int64_t step)
                : from(from), to(to), step(step), i(from) {}

  packToken* next() {
    int64_t value = i;
    if ((step > 0 && value >= to) || (step < 0 && value <= to)) {
      i = from;
      return NULL;
    } else {
      i += step;
      last = static_cast<double>(value);
      return &last;
    }
  }

  void reset() { i=from; }
};

Iterator* Range::getIterator() {
  return new RangeIterator(from, to, step);
}

/* * * * * Built-in range function: * * * * */

const char* range_args[] = {"from", "to", "step"};
packToken default_range(packMap scope) {
  int64_t to, step, from;

  packToken* p_from = scope->find("from");
  packToken* p_to = scope->find("to");
  packToken* p_step = scope->find("step");

  if ((*p_from)->type == NUM) {
    from = p_from->asDouble();
  } else if ((*p_from)->type == NONE) {
    throw std::invalid_argument("range() expects at least 1 argument!");
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

  return packToken(new Token<Range>(Range(from, to, step), IT));
}

/* * * * * Range Startup class * * * * */

struct Range::Startup {
  Startup() {
    TokenMap& global = TokenMap::default_global();
    global["range"] = CppFunction(default_range, 3, range_args, "range");
  }
} iterator_startup;
