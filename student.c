#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STUD_FILE "students.txt"
#define CRE_FILE  "credentials.txt"

char currentUser[50];
char currentRole[20];

/* ---------- Helper functions ---------- */

int equalsIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++;
        b++;
    }
    return *a == *b;
}

/* ---------- Login System ---------- */

int login() {
    char u[50], p[50], r[20];
    char inUser[50], inPass[50];

    int attempts = 0;

    while (attempts < 3) {
        printf("\nLOGIN (%d/3)\n", attempts + 1);
        printf("USERNAME: ");
        scanf("%49s", inUser);
        printf("PASSWORD: ");
        scanf("%49s", inPass);

        FILE *fp = fopen(CRE_FILE, "r");
        if (!fp) {
            printf("Credential file '%s' missing!\n", CRE_FILE);
            return 0;
        }

        int found = 0;
        while (fscanf(fp, "%49s %49s %19s", u, p, r) == 3) {
            if (strcmp(inUser, u) == 0 && strcmp(inPass, p) == 0) {
                strcpy(currentUser, u);
                strcpy(currentRole, r);
                found = 1;
                break;
            }
        }

        fclose(fp);

        if (found) {
            return 1;  // successful login
        } else {
            printf("Invalid username/password!\n");
            attempts++;
        }
    }

    printf("Too many failed attempts. Exiting.\n");
    return 0;
}

/* ---------- Student Operations ---------- */

int rollExists(int roll) {
    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) return 0; // file doesn't exist yet, so no roll

    int r;
    char name[50];
    float mark;

    while (fscanf(fp, "%d %49s %f", &r, name, &mark) == 3) {
        if (r == roll) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void addStudent() {
    int roll;
    char name[50];
    float mark;

    printf("Roll: ");
    scanf("%d", &roll);

    if (rollExists(roll)) {
        printf("A student with roll %d already exists! Try a different roll.\n", roll);
        return;
    }

    printf("Name (single word, no spaces): ");
    scanf("%49s", name);
    printf("Mark: ");
    scanf("%f", &mark);

    FILE *fp = fopen(STUD_FILE, "a");
    if (!fp) {
        printf("Error opening student file '%s'!\n", STUD_FILE);
        return;
    }

    fprintf(fp, "%d %s %.2f\n", roll, name, mark);
    fclose(fp);

    printf("Student added successfully!\n");
}

void displayStudents() {
    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) {
        printf("No student file found! Add some students first.\n");
        return;
    }

    int roll;
    char name[50];
    float mark;
    int count = 0;

    printf("\n%-10s %-20s %-10s\n", "Roll", "Name", "Mark");
    printf("---------------------------------------------\n");
    while (fscanf(fp, "%d %49s %f", &roll, name, &mark) == 3) {
        printf("%-10d %-20s %-10.2f\n", roll, name, mark);
        count++;
    }

    if (count == 0) {
        printf("No student records found.\n");
    } else {
        printf("\nTotal students: %d\n", count);
    }

    fclose(fp);
}

void searchStudentByRoll() {
    int find, roll;
    char name[50];
    float mark;
    int found = 0;

    printf("Enter roll to search: ");
    scanf("%d", &find);

    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) {
        printf("No student file found!\n");
        return;
    }

    while (fscanf(fp, "%d %49s %f", &roll, name, &mark) == 3) {
        if (roll == find) {
            printf("Found: Roll=%d Name=%s Mark=%.2f\n", roll, name, mark);
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("Student with roll %d not found!\n", find);
    }
}

void searchStudentByName() {
    char query[50];
    int roll;
    char name[50];
    float mark;
    int found = 0;

    printf("Enter name to search (single word): ");
    scanf("%49s", query);

    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) {
        printf("No student file found!\n");
        return;
    }

    printf("\nSearch results for name '%s':\n", query);
    printf("%-10s %-20s %-10s\n", "Roll", "Name", "Mark");
    printf("---------------------------------------------\n");

    while (fscanf(fp, "%d %49s %f", &roll, name, &mark) == 3) {
        if (equalsIgnoreCase(name, query)) {
            printf("%-10d %-20s %-10.2f\n", roll, name, mark);
            found = 1;
        }
    }

    fclose(fp);

    if (!found) {
        printf("No student found with that name.\n");
    }
}

void deleteStudent() {
    int delRoll;
    printf("Enter roll to delete: ");
    scanf("%d", &delRoll);

    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) {
        printf("No student file found!\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        printf("Error opening temporary file!\n");
        fclose(fp);
        return;
    }

    int roll;
    char name[50];
    float mark;
    int found = 0;

    while (fscanf(fp, "%d %49s %f", &roll, name, &mark) == 3) {
        if (roll != delRoll) {
            fprintf(temp, "%d %s %.2f\n", roll, name, mark);
        } else {
            found = 1;
        }
    }

    fclose(fp);
    fclose(temp);

    remove(STUD_FILE);
    rename("temp.txt", STUD_FILE);

    if (found) printf("Student with roll %d deleted successfully!\n", delRoll);
    else printf("Roll %d not found!\n", delRoll);
}

void updateStudent() {
    int updateRoll;
    printf("Enter roll to update: ");
    scanf("%d", &updateRoll);

    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) {
        printf("No student file found!\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        printf("Error opening temporary file!\n");
        fclose(fp);
        return;
    }

    int roll;
    char name[50];
    float mark;
    int found = 0;

    while (fscanf(fp, "%d %49s %f", &roll, name, &mark) == 3) {
        if (roll == updateRoll) {
            found = 1;
            char newName[50];
            float newMark;

            printf("Current Name: %s, Current Mark: %.2f\n", name, mark);
            printf("New Name (single word): ");
            scanf("%49s", newName);
            printf("New Mark: ");
            scanf("%f", &newMark);

            fprintf(temp, "%d %s %.2f\n", roll, newName, newMark);
        } else {
            fprintf(temp, "%d %s %.2f\n", roll, name, mark);
        }
    }

    fclose(fp);
    fclose(temp);

    remove(STUD_FILE);
    rename("temp.txt", STUD_FILE);

    if (found) printf("Student with roll %d updated successfully!\n", updateRoll);
    else printf("Roll %d not found!\n", updateRoll);
}

/* ---------- Statistics ---------- */

void showStatistics() {
    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) {
        printf("No student file found!\n");
        return;
    }

    int roll;
    char name[50];
    float mark;

    int count = 0;
    float sum = 0.0f;
    float maxMark = -1.0f;
    float minMark = 101.0f;
    int maxRoll = -1, minRoll = -1;
    char maxName[50] = "", minName[50] = "";

    while (fscanf(fp, "%d %49s %f", &roll, name, &mark) == 3) {
        count++;
        sum += mark;

        if (mark > maxMark) {
            maxMark = mark;
            maxRoll = roll;
            strcpy(maxName, name);
        }
        if (mark < minMark) {
            minMark = mark;
            minRoll = roll;
            strcpy(minName, name);
        }
    }

    fclose(fp);

    if (count == 0) {
        printf("No student records to show statistics.\n");
        return;
    }

    printf("\nSTATISTICS\n");
    printf("----------\n");
    printf("Total Students   : %d\n", count);
    printf("Average Mark     : %.2f\n", sum / count);
    printf("Highest Mark     : %.2f (Roll=%d, Name=%s)\n", maxMark, maxRoll, maxName);
    printf("Lowest Mark      : %.2f (Roll=%d, Name=%s)\n", minMark, minRoll, minName);
}

/* ---------- Change Password ---------- */

void changePassword() {
    char oldPass[50], newPass[50], confirmPass[50];
    char u[50], p[50], r[20];

    printf("Change password for user: %s\n", currentUser);

    printf("Enter current password: ");
    scanf("%49s", oldPass);
    printf("Enter new password: ");
    scanf("%49s", newPass);
    printf("Confirm new password: ");
    scanf("%49s", confirmPass);

    if (strcmp(newPass, confirmPass) != 0) {
        printf("New password and confirm password do not match!\n");
        return;
    }

    FILE *fp = fopen(CRE_FILE, "r");
    if (!fp) {
        printf("Credential file '%s' not found!\n", CRE_FILE);
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        printf("Error opening temporary file!\n");
        fclose(fp);
        return;
    }

    int updated = 0;

    while (fscanf(fp, "%49s %49s %19s", u, p, r) == 3) {
        if (strcmp(u, currentUser) == 0 && strcmp(p, oldPass) == 0) {
            fprintf(temp, "%s %s %s\n", u, newPass, r);
            updated = 1;
        } else {
            fprintf(temp, "%s %s %s\n", u, p, r);
        }
    }

    fclose(fp);
    fclose(temp);

    remove(CRE_FILE);
    rename("temp.txt", CRE_FILE);

    if (updated) {
        printf("Password changed successfully!\n");
    } else {
        printf("Current password incorrect. Password not changed.\n");
    }
}

/* ---------- Menus ---------- */

void adminMenu() {
    int c;
    while (1) {
        printf("\n=== ADMIN MENU ===\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student by Roll\n");
        printf("4. Search Student by Name\n");
        printf("5. Update Student\n");
        printf("6. Delete Student\n");
        printf("7. Show Statistics\n");
        printf("8. Change Password\n");
        printf("9. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &c);

        switch (c) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudentByRoll(); break;
            case 4: searchStudentByName(); break;
            case 5: updateStudent(); break;
            case 6: deleteStudent(); break;
            case 7: showStatistics(); break;
            case 8: changePassword(); break;
            default: return; // Logout
        }
    }
}

void staffMenu() {
    int c;
    while (1) {
        printf("\n=== STAFF MENU ===\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student by Roll\n");
        printf("4. Search Student by Name\n");
        printf("5. Update Student\n");
        printf("6. Show Statistics\n");
        printf("7. Change Password\n");
        printf("8. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &c);

        switch (c) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudentByRoll(); break;
            case 4: searchStudentByName(); break;
            case 5: updateStudent(); break;
            case 6: showStatistics(); break;
            case 7: changePassword(); break;
            default: return; // Logout
        }
    }
}

void guestMenu() {
    int c;
    while (1) {
        printf("\n=== GUEST MENU ===\n");
        printf("1. Display All Students\n");
        printf("2. Search Student by Roll\n");
        printf("3. Search Student by Name\n");
        printf("4. Show Statistics\n");
        printf("5. Change Password\n");
        printf("6. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &c);

        switch (c) {
            case 1: displayStudents(); break;
            case 2: searchStudentByRoll(); break;
            case 3: searchStudentByName(); break;
            case 4: showStatistics(); break;
            case 5: changePassword(); break;
            default: return; // Logout
        }
    }
}

/* ---------- main ---------- */

int main() {
    if (!login()) {
        // login() already prints messages
        return 0;
    }

    printf("\nLogin successful! Logged in as ROLE: %s, USER: %s\n",
           currentRole, currentUser);

    if (strcmp(currentRole, "admin") == 0)
        adminMenu();
    else if (strcmp(currentRole, "staff") == 0)
        staffMenu();
    else
        guestMenu();

    printf("\nExiting program. Goodbye!\n");
    return 0;
}
