/*
 * ArgsParser.h
 */

#include <iostream>
#include <string>

#ifndef ARGSPARSER_H_
#define ARGSPARSER_H_


void print_opts(std::map<std::string, std::string>& opts){
	std::cout<<"\n\t=========================\n";
	std::cout<<"\t GraphGrepSX version 3.3\n";
	std::cout<<"\t=========================\n\n";
	std::cout<<"Options:\n";
	for(std::map<std::string, std::string>::iterator IT=opts.begin(); IT!=opts.end(); IT++){
		std::cout<<"\t"<<IT->first<<": "<<IT->second<<"\n";
	}
};



bool opts_contains(std::map<std::string, std::string>& opts, std::string opt){
	return !(opts.find(opt) == opts.end());
};
void opts_add(std::map<std::string, std::string>& opts, std::string opt, std::string value){
	opts.insert(std::pair<std::string, std::string>(opt, value));
};


class TypedOptions{
public:
	enum VERBOSE_TYPE	{ 	VERBOSE_TYPE_STRICT 	= 0,
							VERBOSE_TYPE_VERBOSE 	= 1,
							VERBOSE_TYPE_FULL 		= 2	};
	enum QUERY_TYPE		{ 	QUERY_TYPE_SINGLE,
							QUERY_TYPE_MULTI,
							QUERY_TYPE_DIR	};
	enum MOUTPUT_TYPE	{ 	MOUTPUT_TYPE_NO,
							MOUTPUT_TYPE_SCREEN,
							MOUTPUT_TYPE_FILE	};


public:
	std::string 	db_file;
	QUERY_TYPE 		query_type;
	std::string 	query_file;
	int 			lp;
	VERBOSE_TYPE 	verbose;
	bool 			all_matches;
	MOUTPUT_TYPE 	moutput;
	std::string 	moutput_file;

	TypedOptions(){
		db_file = "";
		query_type = QUERY_TYPE_SINGLE;
		query_file = "";
		lp = 4;
		verbose = VERBOSE_TYPE_STRICT;
		all_matches = true;
		moutput = MOUTPUT_TYPE_NO;
		moutput_file = "";
	};

	bool parse(std::map<std::string, std::string>& opts){
		bool errors = false;

		std::string opt;

		if(opts_contains(opts, "db_file")){
			opt = opts.at("db_file");
			if(opt != "")				db_file = opt;
			else errors = true;
		}

		if(opts_contains(opts, "query_type")){
			opt = opts.at("query_type");
			if(opt == "single")			query_type = QUERY_TYPE_SINGLE;
			else if(opt == "multi")		query_type = QUERY_TYPE_MULTI;
			else if(opt == "dir")		query_type = QUERY_TYPE_DIR;
			else errors = true;
		}

		if(opts_contains(opts, "query_file")){
			opt = opts.at("query_file");
			if(opt != "")				query_file = opt;
			else errors = true;
		}

		if(opts_contains(opts, "lp")){
			opt = opts.at("lp");
			if(atoi(opt.c_str()) > 0) lp = atoi(opt.c_str());
			else	errors = true;
		}

		if(opts_contains(opts, "verbose")){
			opt = opts.at("verbose");
			if(opt == "strict")			verbose = VERBOSE_TYPE_STRICT;
			else if(opt == "verbose")	verbose = VERBOSE_TYPE_VERBOSE;
			else if(opt == "full")		verbose = VERBOSE_TYPE_FULL;
			else errors = true;
		}

		if(opts_contains(opts, "matches")){
			opt = opts.at("matches");
			if(opt == "one")			all_matches = false;
			else if(opt == "all")		all_matches = true;
			else errors = true;
		}

		if(opts_contains(opts, "moutput")){
			opt = opts.at("moutput");
			if(opt == "no")				moutput = MOUTPUT_TYPE_NO;
			else if(opt == "screen")	moutput = MOUTPUT_TYPE_SCREEN;
			else if(opt == "file"){		moutput = MOUTPUT_TYPE_FILE;
				if(opts_contains(opts, "moutput_file")){
					opt = opts.at("moutput_file");
					if(opt != "")				moutput_file = opt;
					else errors = true;
				}
				else errors = true;
			}
			else errors = true;
		}

		return errors;
	}
};


bool parse_build_argv(int argc, char* argv[], int starting_argi, std::map<std::string, std::string>& opts){
	if(argc > starting_argi){
		int argi = starting_argi;
		std::string opt;
		while(argi < argc){
			opt = argv[argi];

			if(opt == "--verbose"){
				if(opts_contains(opts, "verbose"))	return false;
				opts_add(opts, "verbose", "verbose");
			}
			else if(opt == "--full-verbose"){
				if(opts_contains(opts, "verbose"))	return false;
				opts_add(opts, "verbose", "full");
			}
			else if(opt == "--strict"){
				if(opts_contains(opts, "verbose"))	return false;
				opts_add(opts, "verbose", "strict");
			}

			else if(opt == "--lp"){
				if(opts_contains(opts, "lp"))	return false;
				//argc = 5
				// ggsx -b db_file --lp 4
				// 0    1  2       3    4
				argi++;
				if(argc <= argi)	return false;
				if(atoi(argv[argi]) <= 0) return false;
				opts_add(opts, "lp", argv[argi]);
			}

			else{
				return false;
			}

			argi++;
		}
	}

	if(! opts_contains(opts, "lp"))
		opts.insert(std::pair<std::string, std::string>("lp", "4"));
	if(! opts_contains(opts, "verbose"))
		opts.insert(std::pair<std::string, std::string>("verbose", "verbose"));
	return true;
};

bool parse_find_argv(int argc, char* argv[], int starting_argi, std::map<std::string, std::string>& opts){

	if(argc <= starting_argi) return false;

	int argi = starting_argi;
	std::string opt;
	opt = argv[argi];

	if(opt == "--multi"){
		argi++;
		if(argc <= argi)	return false;
		opts_add(opts, "query_type", "multi");
		opts_add(opts, "query_file", argv[argi]);
	}
	else if(opt == "--dir"){
		argi++;
		if(argc <= argi)	return false;
		opts_add(opts, "query_type", "dir");
		opts_add(opts, "query_file", argv[argi]);
	}
	else{
		opts_add(opts, "query_type", "single");
		opts_add(opts, "query_file", argv[argi]);
	}
	argi++;


	while(argi < argc){
		opt = argv[argi];

		if(opt == "--verbose"){
			if(opts_contains(opts, "verbose"))	return false;
			opts_add(opts, "verbose", "verbose");
		}
		else if(opt == "--full-verbose"){
			if(opts_contains(opts, "verbose"))	return false;
			opts_add(opts, "verbose", "full");
		}
		else if(opt == "--strict"){
			if(opts_contains(opts, "verbose"))	return false;
			opts_add(opts, "verbose", "strict");
		}

		else if(opt == "--lp"){
			if(opts_contains(opts, "lp"))	return false;
			argi++;
			if(argc <= argi)	return false;
			if(atoi(argv[argi]) <= 0) return false;
			opts_add(opts, "lp", argv[argi]);
		}

		else if(opt == "--all-matches"){
			if(opts_contains(opts, "matches"))	return false;
			opts_add(opts, "matches", "all");
		}
		else if(opt == "--one-match"){
			if(opts_contains(opts, "matches"))	return false;
			opts_add(opts, "matches", "one");
		}

		else if(opt == "--no-match-output"){
			if(opts_contains(opts, "moutput"))	return false;
			opts_add(opts, "moutput", "no");
		}
		else if(opt == "--screen-match-output"){
			if(opts_contains(opts, "moutput"))	return false;
			opts_add(opts, "moutput", "screen");
		}
		else if(opt == "--file-match-output"){
			if(opts_contains(opts, "moutput"))	return false;
			argi++;
			if(argc <= argi)	return false;
			opts_add(opts, "moutput", "file");
			opts_add(opts, "moutput_file", argv[argi]);
		}



		else{
			return false;
		}

		argi++;
	}

	if(! opts_contains(opts, "lp"))
		opts.insert(std::pair<std::string, std::string>("lp", "4"));
	if(! opts_contains(opts, "verbose"))
		opts.insert(std::pair<std::string, std::string>("verbose", "verbose"));
	if(! opts_contains(opts, "matches"))
		opts.insert(std::pair<std::string, std::string>("matches", "all"));
	if(! opts_contains(opts, "moutput")){
		opts.insert(std::pair<std::string, std::string>("moutput", "file"));
		opts.insert(std::pair<std::string, std::string>("moutput_file", "matches"));
	}
	return true;
};

#endif /* ARGSPARSER_H_ */
