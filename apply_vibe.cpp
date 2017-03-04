#include "all.hpp"

std::map<uint64_t, Note> &apply_vibe(
    const std::map<uint64_t, Note> &notes, double vibe
) {
    static std::map<uint64_t, Note> results;

    results.clear();

    for (const auto &note: notes) {
        results.insert(note);

        results.at(note.first).pitch += vibe;
    }

    return results;
}
