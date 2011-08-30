#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#define COLORIZE
#define COLOR(n) "%{\033[38;5;" n "m%}"

#ifdef COLORIZE
	#define STRCATCOLOR(s, c, len) strlcat(s, COLOR(c), len)
#else
	#define STRCATCOLOR(s, c, len)
#endif

typedef struct {
	char *dir;
	char *abbr;
} PathAlias;

static PathAlias Paths[] = {
	// 1-level home dirs
	{
		.dir = "/Users/farcaller/Developer",
		.abbr = "~DEV",
	},
	{
		.dir = "/Users/farcaller/Documents",
		.abbr = "~DOC",
	},
	{
		.dir = "/Users/farcaller/Downloads",
		.abbr = "~DOW",
	},
	{
		.dir = "/Users/farcaller/Desktop",
		.abbr = "~DSK",
	},
	{
		.dir = "/Users/farcaller/temp",
		.abbr = "~TMP",
	},
	{
		.dir = "/Users/farcaller/Dropbox",
		.abbr = "~DBX",
	},

	// home
	{
		.dir = "/Users/farcaller",
		.abbr = "~",
	},
	
	// terminator
	{
		.dir = NULL,
		.abbr = NULL,
	},
};

PathAlias expand_basepath_abbr(char *path)
{
	int i = 0;
	PathAlias pa = Paths[i];
	while(pa.dir) {
		if(strstr(path, pa.dir) == path)
			break;
		pa = Paths[i++];
	}
	return pa;
}

char *trim_to_subpath(char *path, int depth)
{
	char *subpath = strrchr(path, '/');
	if(subpath) {
		if(depth == 1) {
			return subpath;
		} else {
			char *temp = subpath;
			*temp = '-';
			char *subsubpath = trim_to_subpath(path, depth-1);
			*temp = '/';
			if(subsubpath)
				subpath = subsubpath;
		}
	}
	return subpath;
}

// side effect: would trim path to look correct
char *dev_project_name(char **origpath)
{
	static char projname[MAXPATHLEN];
	static char bufpath[1];
	bzero(projname, MAXPATHLEN);
	bzero(bufpath, 1);
	char *path = *origpath;
	
	if(strlen(path) == 0) {
		return NULL;
	}
	
	char *projpath = strchr(path[0] == '/' ? path+1 : path , '/');
	if(projpath) {
		char *subprojpath = strchr(projpath+1, '/');
		/* now we have
		 * path == root path w/t '/'       Active/Looplr/test/dir
		 * projpath -----------------------------^
		 * subprojpath ---------------------------------^
		 */
		projpath++; // projpath points to project name
		
		if(!subprojpath)
			subprojpath = bufpath;
		*origpath = subprojpath;
		*subprojpath = '\0';
		strlcat(projname, projpath, MAXPATHLEN);
		*subprojpath = '/';
		return projname;
	} else {
		return NULL;
	}
}

int main(int argc, char **argv)
{
	char *path;
	char *outpath = NULL;
	size_t outsize = 0;
	char pathbuf[MAXPATHLEN];
	char *projname = NULL;
	
	// figure the work dir
	if(argc == 1) {
		getcwd(pathbuf, MAXPATHLEN);
		path = pathbuf;
	} else if(argc == 2) {
		path = argv[1];
	} else {
		exit(1);
	}
	
	//printf("[ORIG]%s -> ", path);
	
	// kill the trailing '/' if any
	if(path[strlen(path)-1] == '/' && strlen(path) > 1)
		path[strlen(path)-1] = '\0';
	
	// expand the alias if any
	PathAlias pa = expand_basepath_abbr(path);
	if(pa.dir) {
		outsize = strlen(pa.abbr)+strlen(path)-strlen(pa.dir)+1;
#ifdef COLORIZE
		outsize += 1024; // XXX: dirty hack
#endif
		outpath = malloc(outsize);
		bzero(outpath, outsize);
		STRCATCOLOR(outpath, "015", outsize);
		
		STRCATCOLOR(outpath, "099", outsize);
		strlcat(outpath, pa.abbr, outsize);
		STRCATCOLOR(outpath, "015", outsize);
		path = path+strlen(pa.dir);
		
		// special case for ~DEV
		if(strstr(pa.abbr, "~DEV") == pa.abbr) {
			projname = dev_project_name(&path);
			if(projname) {
				//printf("[PROJTRIM(%s)]%s -> ", projname, path);
				strlcat(outpath, ":", outsize);
				STRCATCOLOR(outpath, "111", outsize);
				strlcat(outpath, projname, outsize);
				STRCATCOLOR(outpath, "015", outsize);
				//strlcat(outpath, "/", outsize);
			}
		}
	} else {
		outsize = strlen(path)+1;
#ifdef COLORIZE
		outsize += 1024; // XXX: dirty hack
#endif
		outpath = malloc(outsize);
		bzero(outpath, outsize);
		STRCATCOLOR(outpath, "015", outsize);
	}
	
	//printf("[SUB]%s -> ", path);
	// trim the rest to 2 dir deep
	char *subpath = trim_to_subpath(path, 2);
	//printf("[ISSUB]%s -> ", subpath);
	if(subpath && (strlen(subpath) != 1 || subpath[0] != '/')) {
		if(strstr(path, subpath) != path) {
			// if we succeed in trimming
			if(projname != NULL) {
				// untrim if proj, we need //
				strlcat(outpath, "/", outsize);
			} else {
				// trim the / otherwise
				subpath++;
			}
		}
		//printf("[TRIM]%s -> ", subpath);
		strlcat(outpath, subpath, outsize);
	} else if(subpath && strlen(subpath) == 1 && subpath[0] == '/' && projname == NULL) {
		strlcat(outpath, subpath, outsize);
	}
	
	//printf("[OUT]%s\n", outpath);
	
	printf("%s", outpath);
	
	free(outpath);
	return 0;
}
