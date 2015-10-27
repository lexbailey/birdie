
#define DEBUG_LEVEL_FUNDAMENTAL (0)
#define DEBUG_LEVEL_BASIC_FLOW (1)
#define DEBUG_LEVEL_FUNC_TRACE (2)
#define DEBUG_LEVEL_FUNC_FLOW (3)
#define DEBUG_LEVEL_FUNC_LINES (4)
#define DEBUG_LEVEL_ALL (5)

#define DEBUG_MAX_LEVEL (-1)
//#define DEBUG_MAX_LEVEL (5)

#define debugFH(file,level,fmt,...) \
            do { if (level <= DEBUG_MAX_LEVEL){char* indent = malloc(sizeof(char)*(level+1)); indent[level] = '\0'; memset(indent, '\t', level); fprintf(stderr, "%s (L%d): %s"fmt"\n", file, level, indent, ## __VA_ARGS__);} } while (0)

#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
#define EXPAND(...) __VA_ARGS__


#define FUNDAMENTAL(fmt,...) DEFER(debugFH(__FILE__,DEBUG_LEVEL_FUNDAMENTAL, fmt, ## __VA_ARGS__))
#define BASIC_FLOW(fmt,...) DEFER(debugFH(__FILE__,DEBUG_LEVEL_BASIC_FLOW, fmt, ## __VA_ARGS__))
#define FUNC_TRACE DEFER(debugFH(__FILE__,DEBUG_LEVEL_FUNC_TRACE, "%s", __func__))
#define FUNC_FLOW(fmt,...) DEFER(debugFH(__FILE__,DEBUG_LEVEL_FUNC_FLOW, "in '%s': " fmt, __func__, ## __VA_ARGS__))
#define FUNC_LINES(fmt,...) DEFER(debugFH(__FILE__,DEBUG_LEVEL_FUNC_LINES,  "in '%s': " fmt, __func__, ## __VA_ARGS__))
#define DEBUG_OTHER(fmt,...) DEFER(debugFH(__FILE__,DEBUG_LEVEL_ALL, fmt, ## __VA_ARGS__))

