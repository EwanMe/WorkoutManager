#include "WorkoutManager.h"
#include <utility>
#include "date.h"

WorkoutManager::WorkoutManager(Point xy, int w, int h, const string& title) 
: Window{xy, w, h, title},
changePage{Point{0,0}, btnW, btnH, "Page", cb_changePage},

day_box{Point{fieldPad, pad}, fieldW/4, fieldH, "Date:"},
month_box{Point{fieldPad+fieldW/4, pad}, fieldW/4, fieldH, ""},
year_box{Point{fieldPad+2*fieldW/4, pad}, fieldW/2, fieldH, ""},
weight_box{Point{fieldPad, pad+2*fieldH}, fieldW, fieldH, "Weight:"},
exercise{Point{fieldPad, pad+4*fieldH}, fieldW, fieldH, "Exercise:"},

newExercise{Point{fieldPad+fieldW+pad/4, pad+3*fieldH}, btnW/2, btnH, "@+", cb_newExercise},
newWorkout{Point{fieldPad, pad+6*fieldH}, btnW*2, btnH, "Add workout", cb_newWorkout},

currentWorkoutBox{Point{fieldPad+fieldW+pad, pad+2*fieldH}, fieldW, fieldH*4, "Current workout"},

changeGraphView{Point{0, graph_y}, btnW, btnH, "View", cb_changeGraphView},

xAxis{Axis::Orientation::x, Point{graph_x, graph_y}, x_length, 31, ""},
yAxis{Axis::Orientation::y, Point{graph_x, graph_y}, y_length, 10, "Weight"},

xAxisLabel{Point{graph_x + x_length/2, graph_y + btnH}, "Last month"}
{
    color(Color::gray); // Background color.
    xAxis.set_color(Color::black);
    // xAxis.label.move();
    yAxis.set_color(Color::black);
    // yAxis.label.move();
    xAxisLabel.set_color(Color::black);
    
    attach(changePage);
    
    attach(day_box);
    attach(month_box);
    attach(year_box);

    attach(exercise);
    attach(weight_box);

    attach(newExercise);
    attach(newWorkout);

    attach(currentWorkoutBox);

    attach(changeGraphView);
    changeGraphView.hide();

    // Adds all available exercises to choice box.
    for (auto e : ExerciseString) {
        exercise.add(e);
    }
}

void WorkoutManager::cb_newExercise(Address, Address pw) {
    reference_to<WorkoutManager>(pw).addExercise();
}

void WorkoutManager::cb_newWorkout(Address, Address pw) {
    reference_to<WorkoutManager>(pw).saveWorkout();
}

void WorkoutManager::cb_changePage(Address, Address pw) {
    reference_to<WorkoutManager>(pw).switchPage();
}

void WorkoutManager::cb_changeGraphView(Address, Address pw) {
    reference_to<WorkoutManager>(pw).cycleView();
}

void WorkoutManager::addExercise() {
    // No need to check for exercise field since there's no blank selection.
    if (weight_box.get_int() > 0) {
        Exercise e = Exercise(exercise.value());
        ExerciseSet es {e, weight_box.get_int()};
        currentWorkout.insert(es);
        
        displayWorkouts();

        // Likewise, no need to clear value for exercise field.
        weight_box.clear_value();
    }
}

void WorkoutManager::saveWorkout() {
    // Checks that there is a date and exercises added.
    date::year y (year_box.get_int());
    date::month m (month_box.get_int());
    date::day d (day_box.get_int());

    date::year_month_day date{y, m, d};

    if (date.ok() && currentWorkout.size() > 0) {
        ofstream ofstr {filename, ios_base::app}; // Appending to file - can store muliple workouts.
        
        if (!ofstr) {
            throw "Can't write to file " + filename; // -- NO CATCH! --
        }

        Workout w {date, currentWorkout};
        ofstr << w;

        day_box.clear_value();
        month_box.clear_value();
        year_box.clear_value();
        currentWorkout.clear();
        displayWorkouts(); // Clears field since currentWorkout is now empty.
    }
}

void WorkoutManager::loadWorkouts() {
    ifstream ifstr {filename};

    // File check.
    if (!ifstr) {
        throw "Can't read from file " + filename; // -- NO CATCH! --
    }

    
    
    // Makes up a date::year_month_day object.
    int d {0};
    int m {0};
    int y {0};

    set<ExerciseSet> exs; // All exercises of a workout.
    string line {""};
    int lineCount {0}; // Iterator.

    // Loads all workouts from file into a set.
    while (getline(ifstr, line)) {
        
        
        // Dates are always on even lines and all exercises on odd lines.
        if (lineCount % 2 == 0) {

            
            stringstream ss;
            int i {0}; // Determines if day, month or year was read.
            for (char c : line) {

                // Full stop indicates separation between dd.mm.yyyy
                if (c == '.') {
                    if (i == 0) {
                        d = stoi(ss.str());
                    }
                    else if (i == 1) {
                        m = stoi(ss.str());
                    }
                    else if (i == 2) {
                        y = stoi(ss.str());
                        
                        break; // Year is read last.
                    }

                    ss.str("");
                    ++i;
                }
                else {
                    ss << c;
                }
            }
        }
        else {

            stringstream ss;
            Exercise ex; // Exercise.
            int w {0}; // Weight.

            for (char c : line) {

                // Comma follows an exercise string.
                if (c == ',') {

                    // This loop gets the index from ExerciseString to look up in Exercise enum.
                    int i {0};
                    for (string s : ExerciseString) {
                        if (ss.str() == s) {
                            ex = Exercise(i);
                            break;
                        }
                        else {
                            ++i;
                        }
                    }

                    ss.str(""); // Empties stream, will be used for other purposes.
                }

                // Semi colon follows weight.
                else if (c == ';') {
                    w = stoi(ss.str()); // Weight is stored as string in file.
                    ss.str("");

                    // Semi colon also signifies the completion of an ExerciseSet, thus
                    // it can be inserted into the set: "exs".
                    ExerciseSet es {ex, w};
                    exs.insert(es);
                }

                // Creates string.
                else {
                    ss << c;
                }
            }

            // When the for-loop is done all exercises are gathered from the given date.
            Workout wo {date::year_month_day{ date::year_month_day {date::year(y), date::month(m), date::day(d)} }, exs};
            loadedWorkouts.insert(wo);

            // Empty for next workout.
            d = 0;
            m = 0;
            y = 0;
            exs.clear();
        }

        ++lineCount; // Iterates to signify odd or even line.
    }
}

void WorkoutManager::displayWorkouts() {
    stringstream ss;
    for (auto s : currentWorkout) {
        ss << s.type << ": " << s.weight << "kg" << endl;
    }
    string out = ss.str();
    currentWorkoutBox.put(out);
}

void WorkoutManager::switchPage() {
    // When on form page, switches to graphs page.
    if (isForm) {
        year_box.hide();
        month_box.hide();
        day_box.hide();
        exercise.hide();
        weight_box.hide();
        newExercise.hide();
        newWorkout.hide();
        currentWorkoutBox.hide();
        
        changeGraphView.show();

        attach(xAxis);
        attach(yAxis);
        attach(xAxisLabel);

        loadWorkouts();
        updateGraphs(true); // Arg. True will draw the graphs.
    }
    // Switches to form page.
    else {
        year_box.show();
        month_box.show();
        day_box.show();
        exercise.show();
        weight_box.show();
        newExercise.show();
        newWorkout.show();
        currentWorkoutBox.show();

        changeGraphView.hide();
        // These classes are not widgets, therefore they must be detached from the window.
        detach(xAxis);
        detach(yAxis);
        detach(xAxisLabel);

        updateGraphs(false); // Arg. False will detach the graphs.
        redraw();
    }

    isForm = !isForm; // Toggles between form and graphs page.
}

void WorkoutManager::updateGraphs(const bool state) {
    if (state) {
        
        // Need to make a selection of workouts to draw graphs from
        // based on what view type the cycleView() function has selected.
        set<Workout> selectedWorkouts;
        
        // Reference point to determine last month, last three months etc.
        auto last = loadedWorkouts.rbegin()->getDate();
        auto lastMonth = stoi(date::format("%m", last.month()));
        auto lastYear = stoi(date::format("%y", last.year()));

        double x_scaler {0}; // Scales x_coord further down the line.
        bool firstIt {true}; // Used for setting x_scaler only on first iteration.

        // Search through all loaded workouts, selcting the specified ones.
        for (Workout w : loadedWorkouts) {
            constexpr int monthLength {31}; // Temp. value, will always be long enough.
            auto monthNum = stoi(date::format("%m", w.getDate().month()));
            auto yearNum = stoi(date::format("%y", w.getDate().year()));

            switch (viewState) {
                
                // TODO: Rework the entire draw functionality.
                // Need to use days and months to plot graph such that it has high resolution.
                // Iterate through each day in each month.
                
                case GraphView::last_month:

                    if (monthNum == lastMonth) {
                        selectedWorkouts.insert(w);
                    }
                    if (firstIt) {
                        x_scaler = (x_length/monthLength);
                        firstIt = false;
                    }
                    break;

                case GraphView::last_three_months:

                    if (monthNum == lastMonth || monthNum == lastMonth-1 || monthNum == lastMonth-2) {
                        selectedWorkouts.insert(w);
                    }
                    if (firstIt) {
                        x_scaler = (x_length/(monthLength*3));
                        firstIt = false;
                    }
                    break;

                case GraphView::last_six_months:

                    if (monthNum == lastMonth || monthNum == lastMonth-1 || monthNum == lastMonth-2 || monthNum == lastMonth-3 || monthNum == lastMonth-4 || monthNum == lastMonth-5) {
                        selectedWorkouts.insert(w);
                    }
                    if (firstIt) {
                        x_scaler = (x_length/(monthLength*6));
                        firstIt = false;
                    }
                    break;

                case GraphView::last_year:

                    if (yearNum == lastYear) {
                        selectedWorkouts.insert(w);
                    }
                    if (firstIt) {
                        x_scaler = (x_length/(monthLength*12));
                        firstIt = false;
                    }
                    break;
            }
        }

        int i {0}; // Iterator used to pick new Color for each unique graph.
        for (Workout w : selectedWorkouts) {
            // Date indicates location on x axis.
            // Each workout type gets its own graph.
            // Each weight of workout indicates location on y axis.
            double x_coord = graph_x + ((x_length/31) * (stoi(date::format("%d", w.getDate().day())) - 1)); // Subtract 1 to start at x=0
            const set<ExerciseSet> exs = w.getResults();
            
            
            for (ExerciseSet es : exs) {

                double y_coord = graph_y - (es.weight * (y_length/100.0));

                // If exercise is found in map, point is added to existing graph.
                if (graphs.find(es.type) != graphs.end()) {
                    
                    graphs.find(es.type)->second->add(Point{static_cast<int>(x_coord), static_cast<int>(y_coord)});
                }
                
                // Exercise not found: creates new graph.
                else {

                    // Using pointers for shapes since the Shape sub-classes lacks copy constructor and copy assign.
                    Open_polyline* opl = new Open_polyline;
                    opl->set_style(Line_style::solid);
                    opl->set_color(Color(i));
                    opl->add(Point{static_cast<int>(x_coord), static_cast<int>(y_coord)}); // Adds new point to graph.

                    graphs.emplace(es.type, opl); // Adds to the map.


                    // Adds an legend to tell what the graphs represent.
                    constexpr int side {10}; // Length of box sides.
                    
                    Rectangle* s = new Rectangle {Point{graph_x + x_length + side, graph_y - y_length + i*(pad/4) - side}, side, side};
                    s->set_color(Color(i));
                    s->set_fill_color(Color(i));

                    Text* l = new Text {Point{graph_x + x_length + side*3, graph_y - y_length + i*(pad/4)}, ExerciseString[es.type]};

                    legend.push_back(s);
                    legend.push_back(l);

                    ++i; // Only need to iterate once a new exercise type is added.
                }           
            }
        }

        
        // Adds all the graphs to the window from the map.
        for (auto it = graphs.begin(); it != graphs.end(); ++it) {
            attach(*it->second);
        }
        
        // Attach all Text and Rectangle objects, making up the legend for the graphs.
        for (Shape* l : legend) {
            attach(*l);
        }


    }
    else {
        // Removes all the graphs from the window via the map.
        for (auto it = graphs.begin(); it != graphs.end(); ++it) {
            detach(*it->second);
            delete it->second;
        }
        graphs.clear(); // Needs to empty map each time since the graph will draw incorrectly otherwise upon redraw.

        for (Shape* l : legend) {
            detach(*l);
            delete l; // New pointers will be made upon reload.
        }
        legend.clear();

        redraw();
    }
}

void WorkoutManager::cycleView() {
    
    switch (viewState) {

        case GraphView::last_month:
            viewState = GraphView::last_three_months;
            xAxisLabel.set_label("Last three months");
            //xAxis.notches = 31;
            break;

        case GraphView::last_three_months:
            viewState = GraphView::last_six_months;
            xAxisLabel.set_label("Last six months");
            break;

        case GraphView::last_six_months:
            viewState = GraphView::last_year;
            xAxisLabel.set_label("Last year");
            break;

        case GraphView::last_year:
            viewState = GraphView::last_month;
            xAxisLabel.set_label("Last month");
            break;

        default:
            viewState = GraphView::last_month; // Tries to fix a broken enum.
            xAxisLabel.set_label("Last month");
            break;

    }
    updateGraphs(true);
}