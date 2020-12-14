#include "Workout.h"
#include "WorkoutManager.h"

int main() {
    try {
        WorkoutManager wm {Point{100,100}, 800, 400, "Workout Manager"};
        gui_main();
    }
    catch(std::runtime_error e) {
        cout << "Runtime error: " << e.what();
        return -1;
    }

	return 0;
}