#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COURSES 10
#define MAX_PREREQ 4
#define MAX_STUDENTS 20
#define MAX_CREDITS 18


typedef struct {
    char code[15];
    char name[50];
    int credits;
    int semester;
    int start;
    int end;
    int prereq[MAX_PREREQ];
    int prereqCount;
} Course;


typedef struct {
    int id;
    char name[50];
    int completed[MAX_COURSES];
    int *plan;
    int planCount;
    int planCapacity;
} Student;


Course courses[MAX_COURSES] = {
    {"C101", "Programming",       4, 1,  9, 11, {-1}, 0},
    {"C102", "Mathematics",       4, 1, 11, 13, {-1}, 0},
    {"C103", "Physics",           3, 1,  9, 10, {-1}, 0},
    {"C104", "Data Structures",   4, 2,  9, 11, {0},  1},
    {"C105", "Digital Logic",     3, 2, 11, 13, {-1}, 0},
    {"C106", "Algorithms",        4, 3,  9, 11, {3},  1},
    {"C107", "Database Systems",  3, 3, 11, 13, {0},  1},
    {"C108", "Advanced Computing",4, 4,  9, 11, {5, 6}, 2},
    {"C109", "Computer Networks", 3, 4, 11, 13, {3},  1},
    {"C110", "AI Fundamentals",   4, 5,  9, 11, {5, 6}, 2}
};

Student students[MAX_STUDENTS];
int studentCount = 0;


void displayCourses(void) {
    int i, j;

    printf("\n========== COURSE CATALOG ==========\n");

    for (i = 0; i < MAX_COURSES; i++) {
        printf("\nCode: %s", courses[i].code);
        printf("\nName: %s", courses[i].name);
        printf("\nCredits: %d", courses[i].credits);
        printf("\nSemester Offered: %d", courses[i].semester);
        printf("\nTime: %d:00 - %d:00",
               courses[i].start, courses[i].end);

        printf("\nPrerequisites: ");

        if (courses[i].prereqCount == 0) {
            printf("None");
        } else {
            for (j = 0; j < courses[i].prereqCount; j++) {
                printf("%s ",
                    courses[courses[i].prereq[j]].code);
            }
        }

        printf("\n------------------------------------\n");
    }
}


int findCourse(char code[]) {
    int i;

    for (i = 0; i < MAX_COURSES; i++) {
        if (strcmp(courses[i].code, code) == 0) {
            return i;
        }
    }

    return -1;
}


void registerStudent(void) {
    Student *s;

    if (studentCount >= MAX_STUDENTS) {
        printf("\nStudent limit reached!\n");
        return;
    }

    s = &students[studentCount];

    printf("\nEnter student ID: ");
    scanf("%d", &s->id);

    printf("Enter student name (no spaces): ");
    scanf("%49s", s->name);

    for (int i = 0; i < MAX_COURSES; i++) {
        s->completed[i] = 0;
    }

    s->plan = NULL;
    s->planCount = 0;
    s->planCapacity = 0;

    studentCount++;

    printf("\nStudent registered successfully!\n");
}


int selectStudent(void) {
    int id, i;

    if (studentCount == 0) {
        printf("\nPlease register a student first.\n");
        return -1;
    }

    printf("\nEnter student ID: ");
    scanf("%d", &id);

    for (i = 0; i < studentCount; i++) {
        if (students[i].id == id) {
            return i;
        }
    }

    printf("\nStudent not found!\n");
    return -1;
}


void markCompleted(void) {
    int s, c;
    char code[15];

    s = selectStudent();

    if (s == -1)
        return;

    printf("Enter completed course code: ");
    scanf("%14s", code);

    c = findCourse(code);

    if (c == -1) {
        printf("\nInvalid course code!\n");
        return;
    }

    students[s].completed[c] = 1;

    printf("\nCourse marked as completed.\n");
}


int prerequisitesMet(Student *s, int c) {
    int i;

    for (i = 0; i < courses[c].prereqCount; i++) {
        int p = courses[c].prereq[i];

        if (!s->completed[p]) {
            return 0;
        }
    }

    return 1;
}


int inPlan(Student *s, int c) {
    int i;

    for (i = 0; i < s->planCount; i++) {
        if (s->plan[i] == c) {
            return 1;
        }
    }

    return 0;
}


void addCourse(void) {
    int s, c, i, totalCredits = 0;
    char code[15];
    Student *st;

    s = selectStudent();

    if (s == -1)
        return;

    st = &students[s];

    printf("Enter course code to add: ");
    scanf("%14s", code);

    c = findCourse(code);

    if (c == -1) {
        printf("\nCourse not found!\n");
        return;
    }

    if (st->completed[c]) {
        printf("\nThis course is already completed.\n");
        return;
    }

    if (inPlan(st, c)) {
        printf("\nCourse already exists in your plan.\n");
        return;
    }

    
    if (!prerequisitesMet(st, c)) {
        printf("\nPrerequisites not satisfied!\n");
        printf("Required courses:\n");

        for (i = 0; i < courses[c].prereqCount; i++) {
            int p = courses[c].prereq[i];

            if (!st->completed[p]) {
                printf("- %s: %s\n",
                       courses[p].code,
                       courses[p].name);
            }
        }

        printf("\nComplete the required courses first.\n");
        return;
    }


    for (i = 0; i < st->planCount; i++) {
        int p = st->plan[i];

        if (courses[c].start < courses[p].end &&
            courses[c].end > courses[p].start) {

            printf("\nScheduling conflict detected!\n");
            printf("Conflicts with %s (%s).\n",
                   courses[p].code,
                   courses[p].name);
            return;
        }
    }

    for (i = 0; i < st->planCount; i++) {
        totalCredits += courses[st->plan[i]].credits;
    }

    if (totalCredits + courses[c].credits > MAX_CREDITS) {
        printf("\nWarning: Credit limit exceeded!\n");
        printf("Maximum recommended credits: %d\n",
               MAX_CREDITS);
        return;
    }

    
    if (st->planCount == st->planCapacity) {
        int newCapacity;

        if (st->planCapacity == 0)
            newCapacity = 2;
        else
            newCapacity = st->planCapacity * 2;

        int *temp = realloc(
            st->plan,
            newCapacity * sizeof(int)
        );

        if (temp == NULL) {
            printf("\nMemory allocation failed!\n");
            return;
        }

        st->plan = temp;
        st->planCapacity = newCapacity;
    }

    st->plan[st->planCount] = c;
    st->planCount++;

    printf("\nCourse added successfully!\n");
}


void displayPlan(void) {
    int s, i, totalCredits = 0;
    Student *st;

    s = selectStudent();

    if (s == -1)
        return;

    st = &students[s];

    printf("\n========== SEMESTER PLAN ==========\n");
    printf("Student: %s\n", st->name);
    printf("Student ID: %d\n", st->id);

    if (st->planCount == 0) {
        printf("No courses added yet.\n");
        return;
    }

    for (i = 0; i < st->planCount; i++) {
        int c = st->plan[i];

        printf("\nCourse: %s", courses[c].code);
        printf("\nName: %s", courses[c].name);
        printf("\nCredits: %d", courses[c].credits);
        printf("\nTime: %d:00 - %d:00\n",
               courses[c].start, courses[c].end);

        totalCredits += courses[c].credits;
    }

    printf("\nTotal courses: %d", st->planCount);
    printf("\nTotal credits: %d\n", totalCredits);

    if (totalCredits > MAX_CREDITS) {
        printf("Warning: Heavy course workload!\n");
    }

    printf("===================================\n");
}


void suggestCourses(void) {
    int s, i, found = 0;
    Student *st;

    s = selectStudent();

    if (s == -1)
        return;

    st = &students[s];

    printf("\n===== AVAILABLE COURSE SUGGESTIONS =====\n");

    for (i = 0; i < MAX_COURSES; i++) {
        if (!st->completed[i] &&
            !inPlan(st, i) &&
            prerequisitesMet(st, i)) {

            printf("%s - %s (%d credits)\n",
                   courses[i].code,
                   courses[i].name,
                   courses[i].credits);

            found = 1;
        }
    }

    if (!found) {
        printf("No additional eligible courses found.\n");
    }
}


int dfs(int node, int state[]) {
    int i;

    
    if (state[node] == 1)
        return 1;

   
    if (state[node] == 2)
        return 0;

    state[node] = 1;

    for (i = 0; i < courses[node].prereqCount; i++) {
        int p = courses[node].prereq[i];

        if (dfs(p, state))
            return 1;
    }

    state[node] = 2;

    return 0;
}


void checkCycles(void) {
    int state[MAX_COURSES] = {0};
    int i;

    for (i = 0; i < MAX_COURSES; i++) {
        if (dfs(i, state)) {
            printf("\nCircular prerequisite dependency detected!\n");
            return;
        }
    }

    printf("\nNo circular prerequisite dependencies found.\n");
}


void displayCompleted(void) {
    int s, i, found = 0;

    s = selectStudent();

    if (s == -1)
        return;

    printf("\n===== COMPLETED COURSES =====\n");

    for (i = 0; i < MAX_COURSES; i++) {
        if (students[s].completed[i]) {
            printf("%s - %s\n",
                   courses[i].code,
                   courses[i].name);
            found = 1;
        }
    }

    if (!found) {
        printf("No completed courses recorded.\n");
    }
}


int main(void) {
    int choice;

    do {
        printf("\n\n========================================");
        printf("\n STUDENT COURSE PLANNER");
        printf("\n========================================");
        printf("\n1. Display Course Catalog");
        printf("\n2. Register Student");
        printf("\n3. Mark Course as Completed");
        printf("\n4. Add Course to Semester Plan");
        printf("\n5. Display Semester Plan");
        printf("\n6. Suggest Eligible Courses");
        printf("\n7. Check Prerequisite Cycles");
        printf("\n8. Display Completed Courses");
        printf("\n9. Exit");
        printf("\nEnter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input. Restart the program.\n");
            break;
        }

        switch (choice) {
            case 1:
                displayCourses();
                break;

            case 2:
                registerStudent();
                break;

            case 3:
                markCompleted();
                break;

            case 4:
                addCourse();
                break;

            case 5:
                displayPlan();
                break;

            case 6:
                suggestCourses();
                break;

            case 7:
                checkCycles();
                break;

            case 8:
                displayCompleted();
                break;

            case 9:
                printf("\nExiting Course Planner...\n");
                break;

            default:
                printf("\nInvalid choice! Try again.\n");
        }

    } while (choice != 9);

    
    for (int i = 0; i < studentCount; i++) {
        free(students[i].plan);
    }

    return 0;
}
