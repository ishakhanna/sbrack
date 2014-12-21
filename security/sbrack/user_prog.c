#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *LABELS[3] = {"Public", "Protected", "Private"};

const int ARR_LEN = 20;

char *role_list[20], *uid_list[20];

int print_list();
int duplicate_uid(char *uid);
int load_uid(void);


int load_roles(void){
    char *line;
    FILE *f;
    int j, no_of_roles;
    line = (char *)malloc(1024);
    f = fopen("/etc/role_label", "r");
    if(f == NULL){
        return -1;
    }
    no_of_roles = 0;
    while(fgets(line, 1024, f)){
        j=0;
        while(line[j] != ';'){
            j++;
        }
        role_list[no_of_roles] = malloc(50);
        strncpy(role_list[no_of_roles], line, j);
        no_of_roles++;
    }
    fclose(f);
    free(line);
    return 0;
}

int add_to_list(char *role_name){
    int i =0;
    while(i < sizeof(role_list) && role_list[i] != NULL){
        i++;
    }
    role_list[i] = malloc(50);
    strcpy(role_list[i], role_name);
    return 0;
}

int delete_from_list(char *role_name){
    int i, flag;
    i = 0;
    flag = 0;
    while(i < sizeof(role_list) && role_list[i] != NULL){
        if(strcmp(role_list[i], role_name) == 0){
            flag = 1;
        }
        if(flag == 1){
            role_list[i] = role_list[i+1];
        }
        i++;
    }
    return 0;
    
}

int add_user() {
    char *uid, *line;
    FILE *f;
    int j, no_of_roles, role_menu_option;
    line = (char *)malloc(1024);
    uid = (char *)malloc(5);
    no_of_roles = 0;
    load_roles();
    printf("Please enter the uid of the user ");
    scanf("%s", uid);
    if(duplicate_uid(uid) == 0){
        printf("This user already has a role. Please remove the user from the role and then try again.\n");
        return -1;
    }
    
    printf("Please Select a role from below menu options.\n");
    print_list();
    scanf("%d", &role_menu_option);
    printf("User with id %s is assigned to %s\n", uid, role_list[role_menu_option-1]);
    f = fopen("/etc/uid_role", "a");
    if(f == NULL){
        return -1;
    }
    fprintf(f, "%s;%s\n", uid, role_list[role_menu_option-1]);
    fclose(f);
    free(line);
    free(uid);
    return 0;
}

int remove_user(){
    
    int uid_arr_no, line_num, i, no_of_uid;
    int l1, l2;
    FILE *f, *tmp_f;
    char *line, *uid;
    uid = malloc(10);
    memset(uid, 0, 10);
    l1 = 0;
    l2 = 0;
    
    printf("Please enter the uid of the user ");
    scanf("%s", uid);
    
    no_of_uid = load_uid();
    uid_arr_no = -1;
    i=0;
    while(i < no_of_uid && uid_list[i] != NULL)
    {
        l1 = atoi(uid);
        l2 = atoi(uid_list[i]);
        
        if(l1 == l2){
            uid_arr_no = i;
            break;
        }
        i++;
    }
    
    f = fopen("/etc/uid_role", "r");
    tmp_f = fopen("/etc/tmp", "w");
    if(f == NULL){
        return -1;
    }
    
    line_num = 0;
    line = (char *)malloc(1024);
    
    while(fgets(line, 1024, f)){
        if(line_num != uid_arr_no){
            fprintf(tmp_f,"%s", line);
        }
        line_num++;
    }
    
    fclose(f);
    fclose(tmp_f);
    remove("/etc/uid_role");
    rename("/etc/tmp", "/etc/uid_role");
    printf("user removed from the role successfully\n");
    
    free(line);
    return 0;
    
}

int is_duplicate(char *name){
    int i =0;
    while(i < sizeof(role_list) && role_list[i] != NULL){
	if(strcmp(role_list[i], name) == 0){
            return 0;
        }
        i++;
    }
    return -1;
    
}

int duplicate_uid(char *uid){
    int no;
    int i =0;
    no = load_uid();
    while(i < no && uid_list[i] != NULL){
	if(strcmp(uid_list[i], uid) == 0){
            return 0;
        }
        i++;
    }
    return -1;
}

int add_role(){
    load_roles();
    int arr[5], i, menu_option;
    char *name, *line;
    FILE *f;
    name = malloc(20);
    memset(name, 0, 20);
    line = malloc(1024);
    memset(line, 0, 1024);
    printf("Give the name of the role to be added\n");
    scanf("%s", name);
    if(is_duplicate(name) == 0){
        printf("The role already exists.\n");
        return -1;
    }
    strcpy(line, name);
    strcat(line, ";");
    printf("Select the menu option for the access you want to give to this role.\n");
    for(i=0; i<3; i++){
        printf("%d. %s\n", (i+1), LABELS[i]);
    }
    scanf("%d", &menu_option);
    
    strcat(line, LABELS[menu_option-1]);
    
    line[strlen(line)] = '\n';
    f = fopen("/etc/role_label", "a");
    if(f == NULL){
        return -1;
    }
    fprintf(f, "%s", line);
    fclose(f);
    add_to_list(name);
    return 0;
}

int delete_role()
{
    load_roles();
    char **roles, *line, *role, *line2;
    FILE *f, *tmp_f, *f2;
    int j, no_of_roles, role_menu_option, line_num, fail_flag, a;
    line = (char *)malloc(1024);
    line2 = (char *)malloc(1024);
    roles = (char *)malloc(100);
    role = (char *)malloc(100);
    f = fopen("/etc/role_label", "r");
    if(f == NULL){
        return -1;
    }
    tmp_f = fopen("/etc/tmp", "w");
    if(tmp_f == NULL){
        return -1;
    }
    no_of_roles = 0;
    printf("Please Select a role from below menu options.\n");
    print_list();
    scanf("%d", &role_menu_option);
    
    a = 0;
    fail_flag = 0;
    f2 = fopen("/etc/uid_role", "r");
    while(fgets(line2, 100, f2)){
        a=0;
        while(line2[a] != ';'){
            a++;
        }
        a++;
        strcpy(role, line2+a);
        role[sizeof(role)-1] = NULL; // Removing last null char
        if(strcmp(role, role_list[role_menu_option-1]) == 0){
            printf("Cannot delete role, users exist in the role\n");
            fail_flag = 1;
            break;
        }
    }
    if(fail_flag == 1){
        fclose(f);
        fclose(f2);
        goto free;
    }
    line_num = 1;
    while(fgets(line, 1024, f)){
        if(role_menu_option != line_num){
            fprintf(tmp_f,"%s", line);
        }
        line_num++;
    }
    fclose(f);
    fclose(tmp_f);
    remove("/etc/role_label");
    rename("/etc/tmp", "/etc/role_label");
    printf("Role removed Successfully\n");
    delete_from_list(role_list[role_menu_option-1]);
    
free:
    free(line);
    free(roles);
    free(role);
    return 0;
}

int print_list(){
    int i =0;
    while(i < sizeof(role_list) && role_list[i] != NULL){
        printf("%d %s\n", i+1, role_list[i]);
        i++;
    }
    return 0;
}

int load_uid(void){
	int i;
    for(	i=0; i<ARR_LEN; i++){
        uid_list[i] = NULL;
    }
    
    char *line;
    FILE *f;
    int j, no_of_uid;
    line = (char *)malloc(1024);
    f = fopen("/etc/uid_role", "r");
    if(f == NULL){
        return -1;
    }
    no_of_uid = 0;
    while(fgets(line, 1024, f)){
        j=0;
        while(line[j] != ';'){
            j++;
        }
        uid_list[no_of_uid] = malloc(50);
	memset(uid_list[no_of_uid], 0, 50);
        strncpy(uid_list[no_of_uid], line, j);
        no_of_uid++;
    }
    fclose(f);
    free(line);
    return no_of_uid;
}

int replace_with(char *buf, char old, char new){
    int i=0;
    while(buf[i] != NULL){
        if(buf[i] == old){
            buf[i] = new;
        }
        i++;
    }
}

int print_uid(){
    printf("\n");
    char *line;
    FILE *f;
    line = (char *)malloc(1024);
    f = fopen("/etc/uid_role", "r");
    if(f == NULL){
        return -1;
    }
    while(fgets(line, 1024, f)){
        replace_with(line, ';', ' ');
        printf("%s\n", line);
    }
    fclose(f);
    free(line);
    return 0;
}

int print_roles(){
    printf("\n");
    char *line;
    FILE *f;
    line = (char *)malloc(1024);
    f = fopen("/etc/role_label", "r");
    if(f == NULL){
        return -1;
    }
    while(fgets(line, 1024, f)){
        replace_with(line, ';', ' ');
        printf("%s\n", line);
    }
    fclose(f);
    free(line);
    return 0;
}

int main(){
	int menu_option = 0;
    load_roles();
    while(menu_option != 8){
        printf("Please select 1 from the menu option\n");
        printf("1. Add new Role\n");
        printf("2. Delete existing Role\n");
        printf("3. Add user to a Role\n");
        printf("4. Delete user from a Role\n");
        printf("5. Display all Uid\n");
        printf("6. Display all Roles\n");
        printf("7. Exit\n");
        printf("Enter the menu option  ");
        scanf("%d", &menu_option);
        
        switch(menu_option){
            case(1) :
                add_role();
                break;
            case(2) :
                delete_role();
                break;
            case(3) :
                add_user();
                break;
            case(4) :
                remove_user();
                break;
            case(5) :
                print_uid();
                break;
            case(6) :
                print_roles();
                break;
            case(7) :
                break;
            default :
                printf("Invalid Menu Option\n");
        }
    }
    return 0;
}
