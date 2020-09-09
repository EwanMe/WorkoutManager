#include "Workout.h"
#include "WorkoutManager.h"

int main() {
    WorkoutManager wm {Point{100,100}, 800, 400, "Workout Manager"};
    gui_main();

	return 0;
}