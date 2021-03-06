struct role_label_node {
    char *label;
    char *role;
};

int counter = -1;

struct role_label_node pair_arr[20];

/*int reset_role_label_arr(void){
	int i;
	for(i=0;i<20;i++)
	{
	pair_arr[i].label = NULL;
	pair_arr[i].role = NULL;
	}
	counter = -1;
	return 0;
}*/

int free_role_label_arr(void){
	int i;
	for(i=0;i<= counter;i++)
	{
	kfree(pair_arr[i].label);
	kfree(pair_arr[i].role);
	}
	counter = -1;
	return 0;

}

void print_role_label_list(void){
	int i;
	printk("***Printing the list*****\n");
	for(i = 0; i <= counter; i++){
		printk("%d Uid %s Role %s\n", i, pair_arr[i].role, pair_arr[i].label);
	}

	printk("***End Printing the list*****\n");
}

static int load_role_label(void){
	//reset_role_label_arr();	
	struct file *role_label;
	int bytes;
	char *c, *label, *role;
	mm_segment_t oldfs;
	void *buf = NULL;
	
	buf = kmalloc(PAGE_SIZE, __GFP_WAIT);
	memset(buf,0, PAGE_SIZE);
	role_label = filp_open("/etc/role_label", O_RDONLY, 0);
	if(IS_ERR(role_label)){
		printk("Roles Init Error in opening the file Roles_labels: %d\n", (int)PTR_ERR(role_label));
		return NULL;
	}
	else
	{
		oldfs = get_fs();
		set_fs(KERNEL_DS);

		bytes = role_label->f_op->read(role_label, buf, PAGE_SIZE, &role_label->f_pos);
		set_fs(oldfs);
		filp_close(role_label, NULL);
	if(bytes != 0)
	{

	  	c = NULL;
		label = NULL;
		role = NULL;
		c = strsep(&buf, "\n");
	do
	{
	   role = strsep(&c, ";");
	   label = strsep(&c, ";");
	   counter++;
	   pair_arr[counter].role = kmalloc(50, __GFP_WAIT);
	
	   pair_arr[counter].label = kmalloc(50, __GFP_WAIT);
	
	   memset(pair_arr[counter].label, 0, 50);
	   memset(pair_arr[counter].role, 0, 50);
        
	   strcpy(pair_arr[counter].label, label);
	   strcpy(pair_arr[counter].role, role);
	   c = strsep(&buf, "\n");
	} while((c != NULL) && (strlen(c) > 0));
	}
	}
kfree(buf);
return 0;
}

int is_permitted(char *role, char *label){
	struct role_label_node res_node;
	int res, i, role_label_i, inode_label_i;
	int permitted = -1;
	char *LABELS[] = {"Private", "Protected", "Public"};
	free_role_label_arr();	
	res = load_role_label();
	res_node.label = NULL;
	res_node.role = NULL;
	//print_role_label_list();
	if(IS_ERR(res)){
		printk("Error while loading role label mapping\n");	
		return -1;
	}

	for(i=0;i<=counter;i++){
		if(strcmp(pair_arr[i].role, role) == 0){
			res_node.role = kmalloc(strlen(pair_arr[i].role), __GFP_WAIT);
			memset(res_node.role, 0, strlen(pair_arr[i].role));
			strcpy(res_node.role, pair_arr[i].role);

			res_node.label = kmalloc(strlen(pair_arr[i].label), __GFP_WAIT);
			memset(res_node.label, 0, strlen(pair_arr[i].label));
			strcpy(res_node.label, pair_arr[i].label);

			break;	
		}
	}

	if(res_node.role != NULL){
		role_label_i = -1;
		inode_label_i = -1;
		for(i=0;i<3;i++){
			if(strcmp(res_node.label, LABELS[i]) == 0){
				role_label_i=i;
			}
			if(strcmp(label, LABELS[i]) == 0){
				inode_label_i=i;
			}
			if(role_label_i <= inode_label_i){
				permitted = 0;
			}else{
				permitted = -1;
			}
		}
	}else{
		permitted = 0;
	}

	kfree(res_node.label);
	kfree(res_node.role);
	//free_role_label_arr();
	return permitted;
}

char *get_label_from_role(char *role){
	int res, i;
	char *res_label;	
	free_role_label_arr();
	res = load_role_label();
	//print_role_label_list();
	if(IS_ERR(res)){
		printk("Error while loading role label mapping\n");	
		return ERR_PTR(-ENOENT);
	}

	for(i=0;i<=counter;i++){
		if(strcmp(pair_arr[i].role, role) == 0){
			res_label = kmalloc(strlen(pair_arr[i].label), __GFP_WAIT);
			memset(res_label, 0, strlen(pair_arr[i].label));
			strcpy(res_label, pair_arr[i].label);
			return res_label;	
		}
	}

	return ERR_PTR(-ENOENT);
}
