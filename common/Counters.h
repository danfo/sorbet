#ifndef SORBET_COUNTERS_H
#define SORBET_COUNTERS_H
#include "common/ConstExprStr.h"
#include "common/common.h"
#include <chrono>
#include <string>

namespace sorbet {

constexpr bool enable_counters = debug_mode;

// A common on the use of strings in this API
//
// We have the following goals for our counters API:
//
// (1) It should be very lightweight to add new counters; You should not need to
//     predeclare counters.
// (2) Counters should be cheap to use (fast).
//
// Goal (1) implies an API that takes strings directly, but comparing strings is
// slow, and goal (2) suggests we don't want to compare strings to store a
// counter. In order to meet these conflicting goals, we store strings as `const
// char*`, and and compare by pointer value.
//
// The ConstExprStr class below can only be constructed from a string literal,
// which enforces safety -- Using string literals prevents use-after-free and
// various other memory errors normally associated with raw C strings. The
// compiler will typically deduplicate string literals within a translation
// units (but not necessarily between translation units), but this can't be
// relied upon, so we canonicalize strings when retrieving statistics.

struct CounterImpl;

// forward declarations for classes that need private access to the counter
// implementation
class StatsD;
namespace core {
class Proto;
}
namespace web_tracer_framework {
class Tracing;
}
struct CounterState {
    CounterState();
    ~CounterState();
    CounterState(CounterState &&rhs);
    CounterState &operator=(CounterState &&rhs);

    CounterState(const CounterState &rhs) = delete;

    /** If `true`, then `counters` is null. */
    bool hasNullCounters() const;

private:
    friend CounterState getAndClearThreadCounters();
    friend void counterConsume(CounterState cs);
    friend class core::Proto;
    friend class StatsD;
    friend class sorbet::web_tracer_framework::Tracing;

    CounterState(std::unique_ptr<CounterImpl> counters);
    std::unique_ptr<CounterImpl> counters;
};

struct Counters {
    static const std::vector<std::string> ALL_COUNTERS;
    enum class EventKind {
        Complete,
        Instant,
    };
};

CounterState getAndClearThreadCounters();
void counterConsume(CounterState cs);

void prodCounterInc(ConstExprStr counter);
void prodCounterAdd(ConstExprStr counter, unsigned long value);
void counterInc(ConstExprStr counter);
void counterAdd(ConstExprStr counter, unsigned long value);
void categoryCounterInc(ConstExprStr category, ConstExprStr counter);
void categoryCounterAdd(ConstExprStr category, ConstExprStr counter, unsigned long value);
void prodCategoryCounterInc(ConstExprStr category, ConstExprStr counter);
void prodCategoryCounterAdd(ConstExprStr category, ConstExprStr counter, unsigned long value);
void histogramInc(ConstExprStr histogram, int key);
void histogramAdd(ConstExprStr histogram, int key, unsigned long value);
void prodHistogramInc(ConstExprStr histogram, int key);
void prodHistogramAdd(ConstExprStr histogram, int key, unsigned long value);
/* Does not aggregate over measures, instead, reports them separately.
 * Use with care, as it can make us report a LOT of data. */
struct FlowId {
    int id;
};

void timingAdd(ConstExprStr measure, std::chrono::time_point<std::chrono::steady_clock> start,
               std::chrono::time_point<std::chrono::steady_clock> end,
               std::vector<std::pair<ConstExprStr, std::string>> args, FlowId self, FlowId previous);

void instantAdd(ConstExprStr measure, std::chrono::time_point<std::chrono::steady_clock> start,
                std::vector<std::pair<ConstExprStr, std::string>> args);

UnorderedMap<long, long> getAndClearHistogram(ConstExprStr histogram);
std::string getCounterStatistics(std::vector<std::string> names);

} // namespace sorbet
#endif // SORBET_COUNTERS_H
