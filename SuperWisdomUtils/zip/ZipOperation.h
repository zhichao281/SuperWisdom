#pragma once


#include "../zip/zlib/unzip.h"

#include <string>

class ZipOperation

{

public:

	static bool Unzip();



private:

	static void change_file_date(const char *filename, uLong dosdate, tm_unz tmu_date);

	static int mymkdir(const char* dirname);

	static int makedir(char *newdir);

	static void Display64BitsSize(ZPOS64_T n, int size_char);

	static int do_list(unzFile uf);

	static int do_extract_currentfile(unzFile uf, const int* popt_extract_without_path, int* popt_overwrite, const char* password);

	static int do_extract(unzFile uf, int opt_extract_without_path, int opt_overwrite, const char* password);

	static int do_extract_onefile(unzFile uf, const char* filename, int opt_extract_without_path, int opt_overwrite, const char* password);



};


