int role_counter = -1;

struct role_node {
    char *uid;
    char *role;
};

struct role_node pairs[20];

void print_list(void);

int parse_data(char *buf){
	char *c, *uid, *role;
	c = NULL;
	uid = NULL;
	role = NULL;
	c = strsep(&buf, "\n");
	do
	{
	   uid = strsep(&c, ";");
	   role = strsep(&c, ";");
	  
	role_counter++;
	pairs[role_counter].uid = kmalloc(50, __GFP_WAIT);
	
	pairs[role_counter].role = kmalloc(50, __GFP_WAIT);
	
	memset(pairs[role_counter].uid, 0, 50);
	memset(pairs[role_counter].role, 0, 50);
        
	strcpy(pairs[role_counter].uid, uid);
	strcpy(pairs[role_counter].role, role);
	 
	c = strsep(&buf, "\n");
	} while((c != NULL) && (strlen(c) > 0));
	return 0;
}

int reset_arr(void){
	int i;
for(i=0;i<20;i++)
{
	pairs[i].uid = NULL;
	pairs[i].role = NULL;
}
role_counter = -1;
return 0;
}

int free_arr(void){
	int i;
for(i=0;i<= role_counter;i++)
{
	kfree(pairs[i].uid);
	kfree(pairs[i].role);
}
role_counter = -1;
return 0;

}

int import(void){
	reset_arr();
	struct file *uid_roles;
	int bytes, error;
	mm_segment_t oldfs;
	void *buf = NULL;
	oldfs = get_fs();
	set_fs(KERNEL_DS);

	buf = kmalloc(PAGE_SIZE, __GFP_WAIT);
	memset(buf, 0, PAGE_SIZE);
	uid_roles = filp_open("/etc/uid_role", O_RDONLY, 0);
	
	if(IS_ERR(uid_roles)){
		error = (int) PTR_ERR(uid_roles);
		printk("Roles Init Error in opening the Role UID File : %d\n", error);
		return error;
	}
	
	bytes = uid_roles->f_op->read(uid_roles, buf, PAGE_SIZE, &uid_roles->f_pos);
	set_fs(oldfs);
	filp_close(uid_roles, NULL);

	if(bytes != 0)
	{
		role_counter = -1;
   		parse_data(buf);
	}
	kfree(buf);
	return 0;
}


char *get_role(long uid){
	int i, res;	
	long num;
	char *result;
	free_arr();
	res = import();
	//print_list();
	if(res != 0){
		free_arr();
		return ERR_PTR(-ENOENT);
		//return NULL;
	}
	for(i = 0; i <= role_counter; i++){	
	kstrtol(pairs[i].uid, 10, &num);
	if(num == uid){
		result = kmalloc(strlen(pairs[i].role), __GFP_WAIT);
		memset(result, 0, strlen(pairs[i].role));
		strcpy(result, pairs[i].role);
		//free_arr();
		return result;
	}
	}
	return ERR_PTR(-ENOENT);
	//return NULL;
}

void print_list(void){
int i;
printk("***Printing the list*****\n");
for(i = 0; i <= role_counter; i++){
	printk("%d Uid %s Role %s\n", i, pairs[i].uid, pairs[i].role);
}

printk("***End Printing the list*****\n");
}

