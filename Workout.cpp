#include "Workout.h"

ostream& operator<<(ostream& os, const Exercise e) {
    os << ExerciseString[e];
    return os;
}



bool ExerciseSet::operator<(const ExerciseSet& r) const {
    return r.type > this->type;
}

bool ExerciseSet::operator==(const ExerciseSet& r) const {
    return r.type == this->type;
}



Workout::Workout(date::year_month_day date, set<ExerciseSet> results) : date{date}, results{results} {}

date::year_month_day Workout::getDate() const {
    return date;
}

set<ExerciseSet> Workout::getResults() const {
    return results;
}

bool Workout::operator<(const Workout& r) const {
    return this->date < r.date;
}

// Operator overloading for printing to csv type format.
ostream& operator<<(ostream& os, const Workout& w) {

    // Month has to be converted from letters to numbers.
    os << w.getDate().day() << '.' << date::format("%m", w.getDate().month()) << '.' << w.getDate().year() << ".\n";

    set<ExerciseSet> results = w.getResults();

    for (auto it = results.begin(); it != results.end(); ++it) {
        os << it->type << "," << it->weight << ";";
    }
    os << endl;

    return os;
}