#pragma once

#include <Window.h>
#include <GUI.h> // All widgets.
#include <Graph.h> // All shapes.
#include <string>
#include <map> // For updateGraphs.
#include "Workout.h"

using namespace std;
using namespace Graph_lib;

class WorkoutManager : public Window {

private:
    enum GraphView {
        last_month,
        last_three_months,
        last_six_months,
        last_year
    };

    // Window and widget dimensions.
    static constexpr int pad {60};
    static constexpr int btnW {pad};
    static constexpr int btnH {pad/2};
    static constexpr int fieldW {pad*3};
    static constexpr int fieldH {btnH};
    static constexpr int fieldPad {static_cast<int>(pad*1.5)};

    // Graph placement.
    static constexpr int graph_x {pad};
    int graph_y {y_max()-pad};
    int x_length {x_max()-pad*4};
    int y_length {y_max()-pad*2};

    const string filename {"workouts.csv"};
    bool isForm {true}; // Consider making this an argument to the switchPage() function instead.
    GraphView viewState {GraphView::last_month};


    // --- WIDGETS ---
    Button changePage;

    In_box day_box;
    In_box month_box;
    In_box year_box;
    
    In_box weight_box;

    Choice exercise;

    Button newExercise;
    Button newWorkout;

    Multiline_out_box currentWorkoutBox;

    Button changeGraphView;

    
    // --- SHAPES ---
    Axis xAxis;
    Axis yAxis;

    Text xAxisLabel;

    // Pointers used here to bypass the lack of copy constructor and
    // copy assign in the Open_polyline type.
    map<Exercise, Open_polyline*> graphs; // Graphs for all exercise types.
    vector<Shape*> legend; // Legend showing what the color of the graphs represent.
    

    // --- CONTAINERS ---
    set<ExerciseSet> currentWorkout; // Temporary container for exercises in a workout.
    set<Workout> loadedWorkouts; // Workouts loaded from .cvs. Deleted upon switching to form page.


    // --- CALLBACK FUNCTIONS ---
    static void cb_newExercise(Address, Address pw);
    static void cb_newWorkout(Address, Address pw);
    static void cb_changePage(Address, Address pw);
    static void cb_changeGraphView(Address, Address pw);

public:
    WorkoutManager(Point xy, int w, int h, const string& title);

    void addExercise();
    void saveWorkout();
    void loadWorkouts();

    void displayWorkouts(); // On the form page.

    void switchPage(); // Between form page and graphs page.
    void updateGraphs(const bool state); // state indicates whether graphs should be attached or detached.
    void cycleView();
};