int object_counter = -1;

struct object_label {
    long inode;
    char *label;
    struct object_label *next;
};

struct object_label arr[100];


/*int reset_object_list(void){
	int i;
for(i=0;i<100;i++)
{
	arr[i].inode = NULL;
	arr[i].label = NULL;
}
object_counter = -1;
return 0;
}*/

int free_object_list(void){
	int i;
for(i=0;i<= object_counter;i++)
{
	kfree(arr[i].label);
	arr[i].inode = 0;
}
object_counter = -1;
return 0;

}

int load_file(void){
	//reset_object_list();
	struct file *file_wr;
	char *buf,*c, *ino,*label;;
	int error, bytes_read;
	long i_ino;
	
	mm_segment_t oldfs;
	file_wr = filp_open("/etc/object_label", O_RDONLY, 0);
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	
	if(!file_wr || IS_ERR(file_wr)){
        printk(
               "Error: File cannot be opened\n");
        error = (int) PTR_ERR(file_wr);
        return error;
	}
	buf = kmalloc(PAGE_SIZE, __GFP_WAIT);
	memset(buf, 0, PAGE_SIZE);

    	bytes_read = file_wr->f_op->read(file_wr, buf, PAGE_SIZE, &file_wr->f_pos);
	
	if(bytes_read != 0){	
	c = NULL;
	ino = NULL;
	label = NULL;
	c = strsep(&buf, "\n");
	do
	{
	  i_ino = 0;
	  ino = strsep(&c, ";");
	  label = strsep(&c, ";");
	  kstrtol(ino, 10, &i_ino);
	 
		object_counter++;
		arr[object_counter].label = kmalloc(50, __GFP_WAIT);
	
		memset(arr[object_counter].label, 0, 50);
        
		strcpy(arr[object_counter].label, label);
		arr[object_counter].inode = i_ino;

		 c = strsep(&buf, "\n");
	} while((c != NULL) && (strlen(c) > 0));
	}

	set_fs(oldfs);
	kfree(buf);	

	filp_close(file_wr, NULL);
    
	return 0;   
}


void print_object_list(void){
	int i;
	printk("***Printing the list*****\n");
	for(i = 0; i <= object_counter; i++){
	  printk("%d Inode %lu Label %s\n", i, arr[i].inode, arr[i].label);
	}

	printk("***End Printing the list*****\n");
}

char *get_label(long inode){
	int res, i;
	char *res_label;	
	free_object_list();
	res = load_file();
	if(res != 0){
		free_object_list();
		return ERR_PTR(-ENOENT);
	}
	for(i=0;i<=object_counter;i++){
		if(arr[i].inode == inode){
			res_label = kmalloc(strlen(arr[i].label), __GFP_WAIT);
			memset(res_label, 0, strlen(arr[i].label));
			strcpy(res_label, arr[i].label);
			//free_object_list();
			return res_label;
		}
	}
		
	return ERR_PTR(-ENOENT);
}

int write_to_file(void){
	int error, i;
	char *line, *ino;
	struct file *f;
	mm_segment_t oldfs;
	f = filp_open("/etc/object_label", O_WRONLY|O_CREAT|O_TRUNC, 0);
	oldfs = get_fs();
	set_fs(KERNEL_DS);

	if(!f || IS_ERR(f)){
        error = (int) PTR_ERR(f);
        printk(
               "Error: File cannot be opened %d\n", error);
        return error;
	}	
	ino = kmalloc(20, __GFP_WAIT);	
	line = kmalloc(PAGE_SIZE, __GFP_WAIT);

	for(i=0; i<=object_counter; i++){

	memset(line, 0, PAGE_SIZE);
	memset(ino, 0, 20);

	sprintf(ino, "%d", arr[i].inode);

	strcpy(line, ino);
    
	strcat(line, ";");   

	strcat(line, arr[i].label);
    
	strcat(line, "\n");
    
	vfs_write(f, line, strlen(line), &f->f_pos);

	}
	set_fs(oldfs);
	filp_close(f, NULL);
	kfree(line);
	kfree(ino);

	return 0;
}

int add_entry(long inode, char *label){
	int res;	
	free_object_list();
	res = load_file();
	if(res != 0){
		free_object_list();
		return ERR_PTR(-ENOENT);
	}
	object_counter++;
	arr[object_counter].inode = inode;
	
	arr[object_counter].label = kmalloc(strlen(label), __GFP_WAIT);
	memset(arr[object_counter].label, 0, strlen(label));
	strcpy(arr[object_counter].label, label);
	
	res = write_to_file();
	if(res != 0){
		//free_object_list();
		return ERR_PTR(-ENOENT);
	}

	//free_object_list();
	return 0;
}


int delete_entry(long inode){

	int res, i, j;
	j=-1;	
	free_object_list();
	res = load_file();
	if(res != 0){
		free_object_list();
		return ERR_PTR(-ENOENT);
	}

	for(i=0;i<=object_counter; i++){
		if(arr[i].inode == inode){
			j=i;			
			break;
		}
	}

	if(j == -1){
		//free_object_list();
		return 0;	
	}

	while(j<object_counter){
		strcpy(arr[j].label, arr[j+1].label);
		arr[j].inode = arr[j+1].inode;
		j++;
	}

	arr[object_counter].inode = 0;
	kfree(arr[object_counter].label);
	object_counter--;

	res = write_to_file();
	if(res != 0){
		free_object_list();
		return ERR_PTR(-ENOENT);
	}

	//free_object_list();
	return 0;
}


