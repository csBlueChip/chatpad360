#include <argp.h>     // argp support
#include <stdbool.h>  // bool, true, false
#include <stdlib.h>   // realloc, getenv
#include <string.h>   // strdup, strspn, strcspn

//-----------------------------------------------------------------------------
#define DEBUG 1

#if DEBUG == 1
#	define FDEBUGF(fh,fmt,...) fprintf(fh, fmt, ##__VA_ARGS__)
#	define DEBUGF(fmt,...) fprintf(fmt, ##__VA_ARGS__)
#else
#	define FDEBUGF(...)  do{ }while(0)
#	define DEBUGF(...)  do{ }while(0)
#endif

//-----------------------------------------------------------------------------
#define STREQ(s1,s2)  (strcmp(s1,s2)==0)
#define FREE(p)       do{ free(p);  p = NULL; }while(0)

//-----------------------------------------------------------------------------
#define CLI_NEGATION 1

//-----------------------------------------------------------------------------
// Program name & version
//
#define TOOLNAME  "myApp"
#define VER_MAJ   "1"
#define VER_MIN   "2"
#define VER_SUB   "3"
#define VER_SVN   "4"

//-----------------------------------------------------------------------------
// CLI switches
//
typedef
	struct cli {
		char*   pre;     // tokenised contents of the CLI prefixor

		char*   string;
		char    number;
		int     integer;
		bool    flag1;
		bool    flag2;
		bool    doit;
		bool    secret;
		char*   format;
		int     noise;

		char**  inFName;
		int     inFCnt;
	}
cli_t;

cli_t  cli;

// The content of an environment variable can be forced on the front of 
// the command line - by PREpending the environment variable, anything
// set in the variable can be overridden on the command line
#define ENV_VAR  TOOLNAME"_CLI"

//-----------------------------------------------------------------------------
// argp parameters
//
const char*  argp_program_version = 
	TOOLNAME " " VER_MAJ "." VER_MIN "." VER_SUB "(" VER_SVN ")";

const char*  argp_program_bug_address = "<csbluechip@gmail.com>";

char doc[] = 
	"\nThe contents of the environment variable " ENV_VAR 
		" will be added to the START of the command line arguments "
		"...Use -~ as the FIRST argument to disable this feature."
	"\v"
	"Some suffixing info";

static char args_doc[] = "inputFile";

// http://www.gnu.org/software/libc/manual/html_node/Argp-Option-Vectors.html
// If you can come up with a way to make this readable, you've solved a problem I couldn't!
static struct argp_option options[] = {
	{NULL,      '\0', "",      OPTION_DOC, "SECTION 1 HEADER:", 1},
	// Options within a group are sorted alphabetically at runtime
	// here: b, B, i, n, s
	{"number",   'n', "{0..255}", 0, "Supply a value", 1},
	{"string",   's', "text",  0, "Supply some text", 1},
	// NB. --int, --int=3, and -i all work just fine,  -i 3 doesn't! (logical when you think about it)
	{"int",      'i', "value", OPTION_ARG_OPTIONAL, "Optionally supply a value (if blank, use 99)", 1},
	{"bool-on",  'B', NULL,    0, "Enable a thing (default: disabled)", 1},
	{"bool-off", 'b', NULL,    0, "Disable a thing (default: enabled)", 1},
	{"  Section 1 footer", '\0', "", OPTION_DOC, "", 1},

	// Leave a blank line by incrementing the section number
	// isascii('\x81') == false - so no "short option" for 'doit' or 'secret'
	{"doit", '\x81', NULL, 0, "Do a thing (default: don't do it)", 2},
	{"secret", '\x80', NULL, OPTION_HIDDEN, "Secret hidden option", 2},

	{NULL,     '\0', "",  OPTION_DOC, "ANOTHER SECTION HEADER:", 3},
	{"format",  'f', "tPBpOiosdx", 0, "A lengthy description "
	                                  "{target,Pkts,Bytes,port,Opt,in,out,src,dest,xtra}\n"
	                                  "Note: Pkts, Bytes, and Opt use CAPS", 3},
	{NULL, 'F', NULL, OPTION_ALIAS, NULL, 3},

	// Section #99, leaves gaps for future sections {4..98}
	{NULL,     '\0', "",  OPTION_DOC, "OUTPUT DETAIL:", 99},
	{NULL,     '~', NULL, 0, "Exclude environment variable |"ENV_VAR"|\n"
	                          "This MUST be the FIRST argument", 99},
	{"verbose", 'v', NULL, 0, "Display verbose commentary (-vvv)", 99},
	{"quiet",   'q', NULL, 0, "Do not display a commentary (-qq)", 99},
#if CLI_NEGATION
	{NULL, '!', NULL, OPTION_HIDDEN, "Negate next argument", 99},
#endif

	// A header for the system options
	{NULL, '\0', "", OPTION_DOC, "SYSTEM OPTIONS:", -1},

	{NULL}
};

//+============================================================================
// Parse CLI options
//
static
error_t  cli_parse (int key,  char *arg,  struct argp_state *state)
{
	static int  neg;  // ! (or -!) triggers a logical negation of the next arg

	// http://www.gnu.org/software/libc/manual/html_node/Argp-Parsing-State.html
	// state->input is a pointer to our CLI argument structure
	cli_t*  pCli = state->input;
	bool    special;

	// Process ARGP special cases
	special = true;  // assume special case
	switch (key) {
		// http://www.gnu.org/software/libc/manual/html_node/Argp-Special-Keys.html
		case ARGP_KEY_INIT:  // Inititialise CLI variables
			neg = 0;

			pCli->string  = NULL;
			pCli->number  = 0;
			pCli->integer = 0;
			pCli->flag1   = false;
			pCli->flag2   = true;
			pCli->doit    = false;
			pCli->secret  = false;
			pCli->format  = "";
			pCli->noise   = 2;

			pCli->inFName = NULL;
			pCli->inFCnt  = 0;
			break;

		case ARGP_KEY_FINI:  // Parser has completed
			break;

		case ARGP_KEY_ARG: // arg with no "key" (Eg. input file)
			FDEBUGF(state->err_stream, "KeyArg[%d]=|%s|\n", state->arg_num, arg);
			pCli->inFCnt = state->arg_num + 1;
			pCli->inFName = realloc(pCli->inFName, pCli->inFCnt * sizeof(*pCli->inFName));
			pCli->inFName[state->arg_num] = arg;
			break;

		case ARGP_KEY_NO_ARGS:  // No non-option arguments
			// We demand at least one input filename
			fprintf(state->err_stream, "Input file required.\n");
			argp_usage(state);   // This will exit()

		case ARGP_KEY_END:  // All arguments havce been parsed
			break;

		case ARGP_KEY_SUCCESS:
			break;

		default:
			special = false;
			break;
	}
	if (special)  goto done ;

	// We have chosen to use only key values <= 0xFF
	key &= 0xFF;

	// Debug - track our progress
	FDEBUGF(state->err_stream, "arg%c: ", neg?'~':' ');
	if (isascii(key))  FDEBUGF(state->err_stream, "-%c", key) ;
	else               FDEBUGF(state->err_stream, "-\\x%02X", key) ;
	FDEBUGF(state->err_stream, " => |%s| ... |%s|\n", arg, state->argv[state->next]);

	// Process program specific switches
	switch (key) {
		case '~':    // -~ Ignore environment settings
			fprintf(state->err_stream, "-~ MUST be the FIRST argument.\n");
			exit(1);
			break;

		case '!':    // -~ Negate next option
			if (neg) {
				fprintf(state->err_stream, "Error: Stacked negation\n");
				exit(1);
			}
			neg = 2 ;
			break;

		case 's':    // --secret -s <text>
			pCli->string  = arg;
			break;

		case 'n':    // --number -n {0..255}
			pCli->number  = atoi(arg);
			break;

		case 'i':    // --integer -i [value] ... no -i => 0;  -i => 99;  -i x => x
			pCli->integer = arg ? atoi(arg) : 99;
			break;

		case 'B':    // --bool-on -B
			pCli->flag1 = true;
			break;

		case 'b':     // --bool-off -b
			pCli->flag2 = false;
			break;

		// Bug in gcc refuses to recognise '\x81' !
		case 0x81:  // --doit
			pCli->doit = true;
			break;

		case 0x80:  // --secret
			pCli->secret = true;
			break;

		case 'f':     // --format -f
		case 'F':     // alias -F
			pCli->format  = arg;
			break;

		case 'v':     // --verbose -v
			pCli->noise++;
			break;

		case 'q':     // --quiet -q
			pCli->noise--;
			break;

		default:
			return ARGP_ERR_UNKNOWN;
    }

done:
	if (neg)  neg-- ;
	return 0;
}

//+============================================================================
// Add an environment variable (if any) before argv, and update argc.
// Return the expanded environment variable to be freed later, or NULL
// if no options were added to argv.
//
// add_envopt(&argc, &argv, const char* env)
// On calling, env should be the name of the environment variable to be found
//
static
char*  cli_prefix (int* argcp,  char*** argvp,  const char* pre)
{
	char*   chp   = NULL;    // running pointer through prefix variable
	char**  oargv = NULL;    // runs through old argv array
	char**  nargv = NULL;    // runs through new argv array
	int     oargc = *argcp;  // old argc
	int     nargc = 0;       // number of arguments in prefix
	char*   copy  = NULL;    // copy of the env var
	char*   evsep = " \t";   // parameter separators

	// Sanity checks
	if (!pre || !*pre)  return NULL ;

	// Take a copy of the prefix
	if ( !(copy = strdup(pre)) )  return NULL ;

	// Break the copy in to parameters
	for (chp = copy;  *chp;  nargc++) {
		chp += strspn(chp, evsep);   // skip leading separators
		if (!*chp)  break ;          // end of var check

		chp += strcspn(chp, evsep);  // find end of pram
		if (*chp)  *chp++ = '\0' ;   // mark it
	}

	// No prams?
	if (nargc == 0) {
		FREE(copy);
		return NULL;
	}

	// increase argc
	*argcp += nargc;

	// Allocate the new argv array, with an extra element just in case
	//   the original arg list did not end with a NULL.
	if ( !(nargv = (char**)calloc(*argcp + 1, sizeof(*argvp))) )  return NULL ;
	oargv  = *argvp;
	*argvp = nargv;

	// Copy the program name first
	if (oargc-- < 0)  return NULL ;  // quit("ARGC<=0  eh?");
	*(nargv++) = *(oargv++);

	// Then insert the extra args
	for (chp = copy;  nargc > 0;  nargc--) {
		chp += strspn(chp, evsep);  // skip separators
		*(nargv++) = chp;           // store start
		while (*chp++) ;            // skip over word
	}

	// Finally copy the old args and add a NULL (usual convention)
	while (oargc--)  *(nargv++) = *(oargv++) ;
	*nargv = NULL;

	return copy;  // The caller will need to free() this
}

//+============================================================================
static
void  cli_dump (cli_t*  pCli)
{
	int  i;

	printf("string =|%s|\n",  cli.string);
	printf("number =|%d|\n",  cli.number);
	printf("integer=|%d|\n", cli.integer);
	printf("flag1  =|%s|\n",   cli.flag1?"true":"false");
	printf("flag2  =|%s|\n",   cli.flag2?"true":"false");
	printf("doit   =|%s|\n",    cli.doit?"true":"false");
	printf("secret =|%s|\n",  cli.secret?"true":"false");
	printf("format =|%s|\n",  cli.format);
	printf("noise  =|%d|\n",   cli.noise);

	for (i = 0;  i < cli.inFCnt;  i++)
		printf("infname[%d]=|%s|\n", i, cli.inFName[i]);

	return;
}

//+============================================================================
// Replace all ! with -!
//
static
void  cli_negflg (int argc,  char** argv)
{
	int  i;

	for (i = 1;  i < argc;  i++)
		if (STREQ(argv[i], "!"))  argv[i] = "-!" ;

	return;
}

//+============================================================================
// This is why 'cli_t cli' is global
//
static
void  cleanup (void)
{
	FREE(cli.inFName);

	FREE(cli.pre);

	return;
}

//+============================================================================
int  main (int argc,  char** argv,  char** envv)
{
	int          i;

	struct argp  argp   = {options, cli_parse, args_doc, doc};
	int          badopt = 0;

	// http://www.gnu.org/software/libc/manual/html_node/Argp-Parsing-State.html
	unsigned     flags  = 0;

	// Destructor
	atexit(cleanup);

	// CLI before environment padding
	printf("# User Input: |%s", argv[0]);
	for (i = 1;  i < argc;  i++)  printf("|%s", argv[i]) ;
	printf("|\n");

	// Unless disabled, Add the env var prams IN FRONT OF the argv[] prams
	//   ...This allows command line override of env var settings
	// Remember to free(cli.pre) before exiting
	if ( (argc > 1) && (!STREQ(argv[1], "-~")) ) {  // Don't EXclude env var
		cli.pre = cli_prefix(&argc, &argv, getenv(ENV_VAR));
		printf("# Enviroment: |%s|=|%s|\n",
		       ENV_VAR, cli.pre ? getenv(ENV_VAR) : " -not found- ");

	} else {
		// Strip off the (prefixing) -~
		for (i = 1;  i < argc;  i++)  argv[i] = argv[i+1] ;
		argc--;
	}

	// Preprocess for logical negations
	cli_negflg(argc, argv);

	// CLI after environment padding
	printf("# Command arguments: |%s", argv[0]);
	for (i = 1;  i < argc;  i++)  printf("|%s", argv[i]) ;
	printf("|\n");

	// Parse CLI (&cli is not actually /required/ as it is global)
	printf("-----PARSE-----\n");
	argp_parse(&argp, argc, argv, flags, &badopt, &cli);

	// Dump the results
	printf("-----DUMP-----\n");
	cli_dump(&cli);

	return 0;
}
