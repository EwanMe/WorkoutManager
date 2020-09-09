#pragma once

#include <set>
#include <iostream>
#include <string>
#include <fstream>
#include "date.h" // Substitute for C++20 <chrono> which was not released yet.

using namespace std;

// Enumeration for exercises.
enum Exercise {
    ArnoldPress,
    BenchPress,
    BenchPressIncline,
    BenchPressDecline,
    BicepsCurl,
    CableFlies,
    Deadlift,
    EZBarCurl,
    HammerCurl,
    LatPullDown,
    LatRaise,
    LegCurl,
    LegExt,
    LegPress,
    Pulley,
    ShoulderPress,
    Squat,
    StandingRow,
    TriBenchExt,
    TriPullDownBar,
    TriPullDownRope
};

// Array of strings indexed by Exercise enums.
const char* const ExerciseString[] = {
    "Arnold Press",
    "Bench Press",
    "Bench Press Incline",
    "Bench Press Decline",
    "Biceps Curl",
    "Cable Flies",
    "Deadlift",
    "EZ-Bar Curl",
    "Hammer Curl",
    "Lat Pull Down",
    "Lat Raise",
    "Leg Curl",
    "Leg Extension",
    "Leg Press",
    "Pulley",
    "Shoulder Press",
    "Squat",
    "Standing Row",
    "Triceps Bench Extension",
    "Triceps Pull Down Bar",
    "Triceps Pull Down Rope"
};

// Operator which converts exercise enum into string for output.
ostream& operator<<(ostream& os, const Exercise e);



// Class to give an exercise a corresponding weight.
struct ExerciseSet {
    Exercise type;
    int weight;

    ExerciseSet(Exercise type, int weight) : type{type}, weight{weight} {}

    // Required to add this class to a set.
    bool operator<(const ExerciseSet& r) const;
    bool operator==(const ExerciseSet& r) const;
};



// A collection of all the exercises in a session assigned to a date.
class Workout {
    date::year_month_day date;
    set<ExerciseSet> results;   // Set of all exercises done at given date and weights.

public:
    Workout(date::year_month_day date, set<ExerciseSet> results);

    // Get functions.
    date::year_month_day getDate() const;
    set<ExerciseSet> getResults() const;

    bool operator<(const Workout& r) const; // Needed for use in a std::set.
    // Now csv file is not organized, but upon loading workouts are organized by date.
};

ostream& operator<<(ostream& os, const Workout& w);