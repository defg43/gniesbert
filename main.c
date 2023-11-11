#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>

// A function to calculate the Levenshtein distance between two strings.
int levenshtein_distance(const char *s1, const char *s2) {
  int m = strlen(s1);
  int n = strlen(s2);

  // Create a matrix to store the Levenshtein distances between all prefixes of s1 and s2.
  int **d = malloc((m + 1) * sizeof(int *));
  for (int i = 0; i <= m; i++) {
    d[i] = malloc((n + 1) * sizeof(int));
  }

  // Initialize the first row and column of the matrix.
  for (int i = 0; i <= m; i++) {
    d[i][0] = i;
  }
  for (int j = 0; j <= n; j++) {
    d[0][j] = j;
  }

  // Calculate the Levenshtein distances between all prefixes of s1 and s2.
  for (int i = 1; i <= m; i++) {
    for (int j = 1; j <= n; j++) {
      if (s1[i - 1] == s2[j - 1]) {
        d[i][j] = d[i - 1][j - 1];
      } else {
        d[i][j] = 1 + fmin(d[i - 1][j], fmin(d[i][j - 1], d[i - 1][j - 1]));
      }
    }
  }

  // Return the Levenshtein distance between the last prefixes of s1 and s2.
  return d[m][n];
}

// A function to do fuzzy search on filenames in the current folder and all subfolders.
void fuzzy_search_filenames(const char *search_pattern, const char *current_path) {
  // Open the current directory.
  DIR *dirp = opendir(current_path);
  if (dirp == NULL) {
    perror("opendir");
    return;
  }

  // Read each entry in the directory.
  struct dirent *dp;
  while ((dp = readdir(dirp)) != NULL) {
    // If the entry is a file, then compare it to the search pattern.
    if (dp->d_type == DT_REG) {
      // Calculate the Levenshtein distance between the filename and the search pattern.
      int distance = levenshtein_distance(dp->d_name, search_pattern);

      // If the distance is less than a certain threshold, then the filename is considered a match.
      if (distance < 3) {
        printf("%s\n", dp->d_name);
      }
    }
  }

  // Close the directory.
  closedir(dirp);

  // Recursively search for filenames in subfolders.
  dirp = opendir(current_path);
  while ((dp = readdir(dirp)) != NULL) {
    // If the entry is a directory, then recursively search for filenames in that directory.
    if (dp->d_type == DT_DIR && strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
      char path[1024];
      sprintf(path, "%s/%s", current_path, dp->d_name);
      fuzzy_search_filenames(search_pattern, path);
    }
  }

  // Close the directory.
  closedir(dirp);
}

// The main function.
int main(int argc, char **argv) {
  switch(argc) {
  	case 0:
  	  break;
  	case 1:
	  printf("Usage: %s <search_pattern>\n", argv[0]);
	  break;
  	case 2:
	  fuzzy_search_filenames(argv[1], ".");
	  break;
	case 3:
	  fuzzy_search_filenames(argv[2], argv[1]);
      break;
  }	
  return 0;
}
