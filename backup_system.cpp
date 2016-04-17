#include<iostream>
#include<string>
using namespace std;

// File is taken in form of string, given below is the structure of a file.

struct cvfile{
	string filename;	
	string filecontent;	
	cvfile(string f1,string f2):filename(f1),filecontent(f2){}
};


// Structure for the backed up file at particular timestamp 

struct version{
	int timestamp;
	bool *usedblocks;
	version *next;
};

struct backupfile{
	string filename;
	version *vhead;
	backupfile *next;
};

class backup_disk{
	int total_blocks;
	int size_of_block;
	bool *bitmap;
	int total_block_size;
	char *vdisk;
	int timestamp;
	backupfile **hashmap;
	public:
	backup_disk(int,int);
	int hash(string file);
	void settime(int time);
	void backup(cvfile fileinfo);
	string restore(string file,int time);
	void print_disk();
};


// Function for restoring a file at particular  timestamp.

string backup_disk::restore(string file,int time){
	int hash_val = hash(file);
	string result;
	if(hashmap[hash_val]){
		backupfile *latest = hashmap[hash_val];
		while(latest!=NULL){
			if(latest->filename == file){
				version *current = latest->vhead;
				while(current!=NULL){
					if(current->timestamp <= time){
						for(int i=0;i<total_blocks;i++){
							if(current->usedblocks[i]){
								int k = i*size_of_block;
								for(int j= k;j<k+16;j++)
									result+=vdisk[j];
							}
						}
						break;
					}
					current = current->next;
				}
				if(!result.empty()) return result;
				if(!current) cout<<"No such timestamp recorded";
			}
			latest = latest->next;
		}
		if(latest == NULL)
			cout<<"File not found\n";
	}
	else
		cout<<"File not found\n";
}

void backup_disk::print_disk(){
	int i;
	for(i=0;i<total_block_size;i++) cout<<vdisk[i];
}


// Function for creating a backup

void backup_disk::backup(cvfile fileinfo){
	int hash_val = hash(fileinfo.filename);
	int file_len = fileinfo.filecontent.length();
//	cout<<"file length : "<<file_len<<endl;
	version *current = new version;
	current->timestamp = timestamp;
	current->usedblocks = new bool[size_of_block]();

	current->next = NULL;
	int p = 0;
	for(int i = 0;i<total_blocks;i++){
		if(bitmap[i]){
			bitmap[i] = false;
			int k = i*size_of_block;
			current->usedblocks[i] = true;
			for(int j = k;j<k+16;j++){
				if(p<file_len){
					vdisk[j] = fileinfo.filecontent[p];
					p++;
				}
				else{
					vdisk[j] = '\0'	;
					break;
				}
			}
		}
	}

	backupfile *curr = new backupfile;
	curr->filename = fileinfo.filename;
	curr->vhead = NULL;
	curr->next = NULL;
	backupfile *latest = hashmap[hash_val];
	if(hashmap[hash_val]){
	
		while(latest!=NULL){
			if(latest->filename == fileinfo.filename){
				break;
			}
			latest = latest->next;
		}

		if(latest == NULL){
			backupfile *laser = hashmap[hash_val];
			hashmap[hash_val] = curr;
			curr->next = laser;
			latest = laser;
		}
	}
	else{
		hashmap[hash_val] = curr;
		latest = curr;
	}

	version *temp = latest->vhead;
	latest->vhead = current;
	current->next = temp;
}

void backup_disk::settime(int time){
	timestamp = time;
}

int backup_disk::hash(string file){
	int sum = 0,i;
	for(i=0;i<file.length();i++) sum+=file[i];
}

backup_disk::backup_disk(int total_size,int block_size){
	total_blocks = total_size/block_size;
	total_block_size = total_size;
	size_of_block = block_size;
	vdisk = new char[total_size];
	bitmap = new bool[total_blocks];
	int i;
	for(i=0;i<total_blocks;i++)
		bitmap[i] = true;
	hashmap = new backupfile*[10];
}

int main(){
	string f1_1 = "This is file 1 at timestamp 1";
	string f1_2 = "This is file 1 at timestamp 2";
	string f2_1 = "this is file 2 at timestamp 2";
	string f3_1 = "this is file 3 at timestamp 3";

	backup_disk *dp = new backup_disk(256,16);
	dp->settime(1);
	cvfile fileinfo("f1_1",f1_1);
	dp->backup(fileinfo);
	cout<<dp->restore("f1_1",1);
	//dp->print_disk();
}
