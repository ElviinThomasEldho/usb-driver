#include <linux/module.h>
#include <linux/fs.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elviin Thomas Eldho");
MODULE_DESCRIPTION("Kernel module to manage directory permissions");
MODULE_VERSION("1.0");

static mode_t original_mode;
static int mode_saved = 0;

static int remove_directory_permissions(const char *path)
{
    struct path dir_path;
    struct inode *inode;
    int error = 0;

    // Resolve the path to obtain the directory's inode
    error = kern_path(path, LOOKUP_FOLLOW, &dir_path);
    if (error) {
        printk(KERN_ERR "Failed to resolve path: %s\n", path);
        return error;
    }

    inode = dir_path.dentry->d_inode;
    original_mode = inode->i_mode;
    mode_saved = 1;  // Flag to indicate that the original mode is saved

    // Remove read, write, and execute permissions for all
    inode->i_mode &= ~(S_IRWXU | S_IRWXG | S_IRWXO);

    // Mark the inode as dirty to write changes to disk
    mark_inode_dirty(inode);

    // Release the path
    path_put(&dir_path);

    printk(KERN_INFO "Removed permissions from directory: %s\n", path);

    return 0; // return success
}

static int __init perm_driver_init(void)
{
    const char *directory_path = "/home/elviin/Desktop/test";

    remove_directory_permissions(directory_path);

    return 0;  // return success
}

static void __exit perm_driver_exit(void)
{
    const char *directory_path = "/home/elviin/Desktop/test";
    struct path dir_path;
    struct inode *inode;
    int error = 0;

    // Ensure the original mode was saved before attempting to restore
    if (!mode_saved) {
        printk(KERN_ERR "Original mode not saved. Cannot restore permissions.\n");
        return;
    }

    // Resolve the path to obtain the directory's inode
    error = kern_path(directory_path, LOOKUP_FOLLOW, &dir_path);
    if (error) {
        printk(KERN_ERR "Failed to resolve path: %s\n", directory_path);
        return;
    }

    inode = dir_path.dentry->d_inode;
    inode->i_mode = original_mode;

    // Mark the inode as dirty to write changes to disk
    mark_inode_dirty(inode);

    // Release the path
    path_put(&dir_path);

    printk(KERN_INFO "Restored permissions for directory: %s\n", directory_path);
}

module_init(perm_driver_init);
module_exit(perm_driver_exit);
