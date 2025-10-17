#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_leap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
int day_of_week(int y, int m, int d) {
    static int t[] = {0,3,2,5,0,3,5,1,4,6,2,4};
    if (m < 3) y -= 1;
    return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

int days_in_month(int month, int year) {
    static int mdays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (month == 2) return mdays[1] + is_leap(year);
    return mdays[month-1];
}

int has_reminder(int year, int month, int day) {
    FILE *fp = fopen("reminders.txt", "r");
    if (!fp) return 0;
    char key[11]; 
    snprintf(key, sizeof(key), "%04d-%02d-%02d", year, month, day);
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, key, 10) == 0) { found = 1; break; }
    }
    fclose(fp);
    return found;
}

void print_month(int month, int year) {
    const char *names[] = {"January","February","March","April","May","June",
                           "July","August","September","October","November","December"};
    int start = day_of_week(year, month, 1);
    int dim = days_in_month(month, year);

    printf("     %s %d\n", names[month-1], year);
    printf("Su Mo Tu We Th Fr Sa\n");

    for (int i = 0; i < start; ++i) printf("   ");
    for (int day = 1; day <= dim; ++day) {
        int marked = has_reminder(year, month, day);
        if (marked)
            printf("%2d*", day);     
        else
            printf("%2d ", day);
        if ((start + day) % 7 == 0) printf("\n");
        else printf(" ");
    }
    if ((start + dim) % 7 != 0) printf("\n");
    printf("\n");
    printf("(Days with '*' have reminders)\n\n");
}

void add_reminder() {
    int y, m, d;
    char text[800];
    printf("Enter date to add reminder (YYYY MM DD): ");
    if (scanf("%d %d %d", &y, &m, &d) != 3) { while (getchar()!='\n'); printf("Invalid input.\n"); return; }
    if (y <= 0 || m < 1 || m > 12 || d < 1 || d > days_in_month(m,y)) { printf("Invalid date.\n"); return; }
    while (getchar()!='\n'); 
    printf("Enter reminder text (single line): ");
    if (!fgets(text, sizeof(text), stdin)) { printf("No input.\n"); return; }
    size_t len = strlen(text);
    if (len && text[len-1] == '\n') text[len-1] = '\0';

    FILE *fp = fopen("reminders.txt", "a");
    if (!fp) { printf("Failed to open reminders.txt for writing.\n"); return; }
    fprintf(fp, "%04d-%02d-%02d | %s\n", y, m, d, text);
    fclose(fp);
    printf("Reminder saved for %04d-%02d-%02d\n", y, m, d);
}

void list_reminders_for_date() {
    int y, m, d;
    printf("Enter date to list reminders (YYYY MM DD): ");
    if (scanf("%d %d %d", &y, &m, &d) != 3) { while (getchar()!='\n'); printf("Invalid input.\n"); return; }
    if (y <= 0 || m < 1 || m > 12 || d < 1 || d > days_in_month(m,y)) { printf("Invalid date.\n"); return; }
    char key[11];
    snprintf(key, sizeof(key), "%04d-%02d-%02d", y, m, d);

    FILE *fp = fopen("reminders.txt", "r");
    if (!fp) { printf("No reminders saved yet.\n"); return; }
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, key, 10) == 0) {
            printf("%s", line + 13);
            found = 1;
        }
    }
    fclose(fp);
    if (!found) printf("No reminders for %s\n", key);
}

int main(void) {
    int choice;
    for (;;) {
        printf("\nCalendar & Reminders\n");
        printf("1) Print month\n");
        printf("2) Add reminder\n");
        printf("3) List reminders for a date\n");
        printf("4) Exit\n");
        printf("Choose: ");
        if (scanf("%d", &choice) != 1) { while (getchar()!='\n'); printf("Invalid choice.\n"); continue; }

        if (choice == 1) {
            int month, year;
            printf("Enter month (1-12) and year (e.g. 2025): ");
            if (scanf("%d %d", &month, &year) != 2) { while (getchar()!='\n'); printf("Invalid input.\n"); continue; }
            if (year <= 0 || month < 1 || month > 12) { printf("Invalid month/year.\n"); continue; }
            print_month(month, year);
        } else if (choice == 2) {
            add_reminder();
        } else if (choice == 3) {
            list_reminders_for_date();
        } else if (choice == 4) {
            printf("Goodbye.\n");
            break;
        } else {
            printf("Invalid option.\n");
        }
    }
    return 0;
}