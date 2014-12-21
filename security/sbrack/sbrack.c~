#include "sbrack_headers.h"

#ifdef CONFIG_SECURITY_SBRACK 

static int sbrack_inode_mkdir(struct inode *dir, struct dentry *dentry,
			   umode_t mask)
{
	char *role, *label;
	if(get_current_user()->uid.val >= 1000){
		role = get_role(get_current_user()->uid.val);
		label = get_label(dir->i_ino);
		if(IS_ERR(role) || IS_ERR(label)){
			printk("Error finding role or label\n");
		}
		else {
			if(is_permitted(role, label) == 0){
				kfree(role);
				kfree(label);				
				return 0;
			}else{
				kfree(role);
				kfree(label);
				return -EACCES;
			}
		}
	}
	
	return 0;
}

static int sbrack_inode_rmdir(struct inode *dir, struct dentry *dentry)
{
	char *role, *label;
	if(get_current_user()->uid.val >= 1000){
		role = get_role(get_current_user()->uid.val);
		label = get_label(dentry->d_inode->i_ino);
		if(IS_ERR(role) || IS_ERR(label)){
			printk("Error finding role or label\n");
		}
		else {
			if(is_permitted(role, label) == 0){
				kfree(role);
				kfree(label);
 				delete_entry(dentry->d_inode->i_ino);				
				return 0;
			}else{
				kfree(role);
				kfree(label);
				return -EACCES;
			}
		}
	}
	return 0;
}

static int sbrack_inode_init_security(struct inode *inode, struct inode *dir, const struct qstr *qstr,
				   const char **name, void **value, size_t *len)
{
	long uid = (long)get_current_user()->uid.val;
	char *role, *label;
	if(uid >= 1000){	
		role = get_role(uid);	
		if(IS_ERR(role)){
			printk("Error finding role\n");
		}
		else{
			label = get_label_from_role(role);
			if(IS_ERR(label)){
				printk("Label for Role not found");
				kfree(role);
			}
			else {
				add_entry((long)inode->i_ino, label);
				kfree(label);
				kfree(role);	
			}
		}
	}
	return 0;
}


static int sbrack_inode_create(struct inode *dir, struct dentry *dentry,
                umode_t mask)
{
	char *role, *label;
	if(get_current_user()->uid.val >= 1000){
		role = get_role(get_current_user()->uid.val);
		label = get_label(dir->i_ino);
		if(IS_ERR(role) || IS_ERR(label)){
			printk("Error finding role or label\n");
		}
		else {
			if(is_permitted(role, label) == 0){
				kfree(role);
				kfree(label);				
				return 0;
			}else{
				kfree(role);
				kfree(label);
				return -EACCES;
			}
		}
	}
	
	return 0;    
}



static int sbrack_inode_unlink(struct inode *dir, struct dentry *dentry)
{    
	char *role, *label;
	if(get_current_user()->uid.val >= 1000){
		role = get_role(get_current_user()->uid.val);
		label = get_label(dentry->d_inode->i_ino);
		if(IS_ERR(role) || IS_ERR(label)){
			printk("Error finding role or label\n");
		}
		else {
			if(is_permitted(role, label) == 0){
				kfree(role);
				kfree(label);
 				delete_entry(dentry->d_inode->i_ino);				
				return 0;
			}else{
				kfree(role);
				kfree(label);
				return -EACCES;
			}
		}
	}
	return 0;

}

char *get_label2(long inode){
	int i;
	char *res_label;
	//free_object_list();				
	//res = load_file();
	/*if(res != 0){
		free_object_list();
		return ERR_PTR(-ENOENT);
	}*/
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

char *get_role2(long uid){
	int i;	
	long num;
	char *result;
	//free_arr();
	//res = import();
	//print_list();
	/*if(res != 0){
		free_arr();
		return ERR_PTR(-ENOENT);
		//return NULL;
	}*/
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

int is_permitted2(char *role, char *label){
	struct role_label_node res_node;
	int i, role_label_i, inode_label_i;
	int permitted = -1;
	char *LABELS[] = {"Private", "Protected", "Public"};
	//res = load_role_label();
	res_node.label = NULL;
	res_node.role = NULL;
	/*if(IS_ERR(res)){
		printk("Error while loading role label mapping\n");	
		return -1;
	}*/

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

static int sbrack_inode_permission(struct inode *inode, int mask)
{    
	char *role, *label;
	
	if(counter < 0 || role_counter < 0 || object_counter < 0){
		return 0;
	}	

	if(get_current_user()->uid.val >= 1000 && !S_ISDIR(inode->i_mode)){
		role = get_role2(get_current_user()->uid.val);
		label = get_label2(inode->i_ino);
		if(IS_ERR(role) || IS_ERR(label)){
			printk("Error finding role or label\n");
		}
		else {
			if(is_permitted2(role, label) == 0){
				kfree(role);
				kfree(label);
 				return 0;
			}else{
				kfree(role);
				kfree(label);
				return -EACCES;
			}
		}
	}
	return 0;
}

static struct security_operations sbrack_ops = {
	.name =				"sbrack",
	.inode_init_security =		sbrack_inode_init_security,
	.inode_mkdir =			sbrack_inode_mkdir,
	.inode_rmdir =			sbrack_inode_rmdir,
	.inode_unlink =			sbrack_inode_unlink,
	.inode_create =			sbrack_inode_create,
        .inode_permission =             sbrack_inode_permission

};


static int __init sbrack_init(void){	
	if (register_security(&sbrack_ops))
		panic("sbrack: Unable to register.\n");
	else 
		printk("sbrack: registered.\n");

	return 0;
}

static void __exit sbrack_exit (void)
{
	return;
}



module_init (sbrack_init);
module_exit (sbrack_exit);

MODULE_DESCRIPTION("sbrack");
MODULE_LICENSE("GPL");

#endif /* CONFIG_SECURITY_SBRACK */
