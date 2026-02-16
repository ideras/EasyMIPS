#include "mips32_completion.h"
#include <replxx.hxx>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace Mips32
{

// ============================================================================
// Data Structures for Completion
// ============================================================================

/**
 * Instruction metadata for autocompletion
 * Organized alphabetically for efficient searching
 */
static const char* instruction_list[] = {
    "add", "addi", "addiu", "addu", "and", "andi",
    "beq", "beqz", "bgez", "bgtz", "blez", "bltz", "bne", "bnez", "break",
    "div", "divu",
    "j", "jal", "jalr", "jr",
    "la", "lb", "lbu", "lh", "lhu", "li", "lui", "lw", "lwc1",
    "mfhi", "mflo", "move", "mthi", "mtlo", "mult", "multu",
    "nop", "nor",
    "or", "ori",
    "sb", "sh", "sll", "sllv", "slt", "slti", "sltiu", "sltu", "sra", "srav",
    "srl", "srlv", "sub", "subu", "sw", "swc1", "syscall",
    "xor", "xori"
};

static const int instruction_count = sizeof(instruction_list) / sizeof(instruction_list[0]);

/**
 * Register metadata for autocompletion
 * Each entry: {name1, name2} where name1 is $rN and name2 is the symbolic name
 */
struct RegInfo
{
    const char *name;       // Primary name (symbolic like $t0, $zero)
    const char *alt_name;   // Alternative name ($r0, $r1, etc.)
};

static const RegInfo register_list[] = {
    {"$zero", "$r0"},
    {"$at", "$r1"},
    {"$v0", "$r2"}, {"$v1", "$r3"},
    {"$a0", "$r4"}, {"$a1", "$r5"}, {"$a2", "$r6"}, {"$a3", "$r7"},
    {"$t0", "$r8"}, {"$t1", "$r9"}, {"$t2", "$r10"}, {"$t3", "$r11"},
    {"$t4", "$r12"}, {"$t5", "$r13"}, {"$t6", "$r14"}, {"$t7", "$r15"},
    {"$s0", "$r16"}, {"$s1", "$r17"}, {"$s2", "$r18"}, {"$s3", "$r19"},
    {"$s4", "$r20"}, {"$s5", "$r21"}, {"$s6", "$r22"}, {"$s7", "$r23"},
    {"$t8", "$r24"}, {"$t9", "$r25"},
    {"$k0", "$r26"}, {"$k1", "$r27"},
    {"$gp", "$r28"},
    {"$sp", "$r29"},
    {"$fp", "$r30"},
    {"$ra", "$r31"},
    {"$lo", "$r32"},
    {"$hi", "$r33"},
    {"$pc", "$r34"}
};

static const int register_count = sizeof(register_list) / sizeof(register_list[0]);

/**
 * Assembler directives for autocompletion
 */
static const char* directive_list[] = {
    ".data", ".text", ".globl", ".word", ".half", ".byte", ".space", ".ascii", ".asciiz", ".align"
};

static const int directive_count = sizeof(directive_list) / sizeof(directive_list[0]);

/**
 * Debugger commands for autocompletion
 */
static const char* command_list[] = {
    "#show", "#set", "#exec", "#reset", "#stop", "#hihw", "#lohw", "#debug"
};

static const int command_count = sizeof(command_list) / sizeof(command_list[0]);

/**
 * Debugger command keywords for autocompletion
 */
static const char* keyword_list[] = {
    "hexadecimal", "hex", "decimal", "signed", "unsigned", "binary",
    "byte", "ascii", "hword", "word", "sep"
};

static const int keyword_count = sizeof(keyword_list) / sizeof(keyword_list[0]);

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * Convert string to lowercase for case-insensitive comparison
 */
static std::string toLower(const std::string &s)
{
    std::string result = s;
    for (char &c : result)
    {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return result;
}

/**
 * Check if a string starts with a prefix (case-insensitive)
 */
static bool startsWithCaseInsensitive(const std::string &str, const std::string &prefix)
{
    if (prefix.length() > str.length())
        return false;
    
    for (size_t i = 0; i < prefix.length(); ++i)
    {
        if (std::tolower(static_cast<unsigned char>(str[i])) !=
            std::tolower(static_cast<unsigned char>(prefix[i])))
        {
            return false;
        }
    }
    return true;
}

/**
 * Check if character is a whitespace
 */
static bool isWhitespace(char c)
{
    return c == ' ' || c == '\t';
}

/**
 * Check if character is valid for an identifier
 */
static bool isIdentifierChar(char c)
{
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.';
}

/**
 * Check if character could start an identifier
 */
static bool isIdentifierStartChar(char c)
{
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '.' || c == '$';
}

// ============================================================================
// Line Parsing
// ============================================================================

/**
 * Parse the input line to extract tokens and determine completion context
 */
LineState parseLine(const std::string &line, int cursor_pos)
{
    LineState state;
    state.line = line;
    state.cursor_pos = cursor_pos;
    state.context = CompletionContext::Unknown;
    
    // Find start of current token by scanning backwards from cursor
    int start = cursor_pos;
    while (start > 0)
    {
        char c = line[start - 1];
        if (isWhitespace(c) || c == ',' || c == '(' || c == ')' || c == '=' || c == '[' || c == ']')
        {
            break;
        }
        start--;
    }
    
    state.current_token.start_pos = start;
    state.current_token.end_pos = cursor_pos;
    state.current_token.text = line.substr(start, cursor_pos - start);
    
    // Identify all complete tokens before the current one for context
    std::string before = line.substr(0, start);
    std::string current_tok;
    bool in_tok = false;
    
    for (char c : before)
    {
        if (isWhitespace(c) || c == ',' || c == '(' || c == ')' || c == '=' || c == '[' || c == ']')
        {
            if (in_tok)
            {
                state.tokens.push_back(current_tok);
                current_tok.clear();
                in_tok = false;
            }
        }
        else
        {
            current_tok += c;
            in_tok = true;
        }
    }
    if (in_tok)
    {
        state.tokens.push_back(current_tok);
    }

    // Determine context based on current token and previous tokens
    if (state.current_token.text.empty())
    {
        // Cursor is after a space or separator
        if (state.tokens.empty())
        {
            state.context = CompletionContext::Instruction;
        }
        else
        {
            const std::string &last_token = state.tokens.back();
            if (last_token == "#show" || last_token == "#set")
            {
                state.context = CompletionContext::Keyword;
            }
            else if (last_token[0] == '$')
            {
                state.context = CompletionContext::Register;
            }
            else if (last_token[0] == '.')
            {
                state.context = CompletionContext::Unknown;
            }
            else if (last_token[0] == '#')
            {
                state.context = CompletionContext::Unknown;
            }
            else
            {
                state.context = CompletionContext::Register;
            }
        }
    }
    else if (state.current_token.text[0] == '$')
    {
        state.context = CompletionContext::Register;
    }
    else if (state.current_token.text[0] == '.')
    {
        state.context = CompletionContext::Directive;
    }
    else if (state.current_token.text[0] == '#')
    {
        state.context = CompletionContext::Command;
    }
    else if (state.tokens.empty())
    {
        state.context = CompletionContext::Instruction;
    }
    else
    {
        // Check if we are likely in a command that expects keywords
        bool in_keyword_cmd = false;
        for (const auto& t : state.tokens)
        {
            if (t == "#show" || t == "#set")
            {
                in_keyword_cmd = true;
                break;
            }
        }

        if (in_keyword_cmd)
            state.context = CompletionContext::Keyword;
        else
            state.context = CompletionContext::Instruction;
    }
    
    return state;
}

// ============================================================================
// Completion Functions
// ============================================================================

/**
 * Get instruction completions matching a prefix
 */
std::vector<std::string> completeInstruction(const std::string &prefix)
{
    std::vector<std::string> matches;
    std::string lower_prefix = toLower(prefix);
    
    for (int i = 0; i < instruction_count; ++i)
    {
        if (startsWithCaseInsensitive(instruction_list[i], lower_prefix))
        {
            matches.push_back(instruction_list[i]);
        }
    }
    
    // Sort alphabetically
    std::sort(matches.begin(), matches.end());
    
    return matches;
}

/**
 * Get register completions matching a prefix
 */
std::vector<std::string> completeRegister(const std::string &prefix)
{
    std::vector<std::string> matches;
    std::string lower_prefix = toLower(prefix);
    
    for (int i = 0; i < register_count; ++i)
    {
        // Check primary name (symbolic like $t0)
        if (startsWithCaseInsensitive(register_list[i].name, lower_prefix))
        {
            matches.push_back(register_list[i].name);
        }
        // Check alternative name ($r0, $r1, etc.)
        else if (startsWithCaseInsensitive(register_list[i].alt_name, lower_prefix))
        {
            matches.push_back(register_list[i].alt_name);
        }
    }
    
    // Sort alphabetically
    std::sort(matches.begin(), matches.end());
    
    return matches;
}

/**
 * Get directive completions matching a prefix
 */
std::vector<std::string> completeDirective(const std::string &prefix)
{
    std::vector<std::string> matches;
    std::string lower_prefix = toLower(prefix);
    
    for (int i = 0; i < directive_count; ++i)
    {
        if (startsWithCaseInsensitive(directive_list[i], lower_prefix))
        {
            matches.push_back(directive_list[i]);
        }
    }
    
    // Sort alphabetically
    std::sort(matches.begin(), matches.end());
    
    return matches;
}

/**
 * Get command completions matching a prefix
 */
std::vector<std::string> completeCommand(const std::string &prefix)
{
    std::vector<std::string> matches;
    std::string lower_prefix = toLower(prefix);
    
    for (int i = 0; i < command_count; ++i)
    {
        if (startsWithCaseInsensitive(command_list[i], lower_prefix))
        {
            matches.push_back(command_list[i]);
        }
    }
    
    // Sort alphabetically
    std::sort(matches.begin(), matches.end());
    
    return matches;
}

/**
 * Get keyword completions matching a prefix
 */
std::vector<std::string> completeKeyword(const std::string &prefix)
{
    std::vector<std::string> matches;
    std::string lower_prefix = toLower(prefix);
    
    for (int i = 0; i < keyword_count; ++i)
    {
        if (startsWithCaseInsensitive(keyword_list[i], lower_prefix))
        {
            matches.push_back(keyword_list[i]);
        }
    }
    
    // Sort alphabetically
    std::sort(matches.begin(), matches.end());
    
    return matches;
}

// ============================================================================
// Main Completion Logic
// ============================================================================

/**
 * Complete the line based on context and return matches
 */
std::vector<std::string> completeLine(const std::string &line, int cursor_pos)
{
    LineState state = parseLine(line, cursor_pos);
    
    std::vector<std::string> matches;
    
    switch (state.context)
    {
        case CompletionContext::Instruction:
            matches = completeInstruction(state.current_token.text);
            // Also suggest commands if prefix is empty or starts with #
            if (state.current_token.text.empty() || state.current_token.text[0] == '#')
            {
                std::vector<std::string> cmd_matches = completeCommand(state.current_token.text);
                matches.insert(matches.end(), cmd_matches.begin(), cmd_matches.end());
            }
            break;
            
        case CompletionContext::Register:
            matches = completeRegister(state.current_token.text);
            break;
            
        case CompletionContext::Directive:
            matches = completeDirective(state.current_token.text);
            break;

        case CompletionContext::Command:
            matches = completeCommand(state.current_token.text);
            break;

        case CompletionContext::Keyword:
            matches = completeKeyword(state.current_token.text);
            break;
            
        case CompletionContext::Label:
        case CompletionContext::Unknown:
        default:
            // Try everything as fallback
            matches = completeInstruction(state.current_token.text);
            {
                std::vector<std::string> cmd_matches = completeCommand(state.current_token.text);
                matches.insert(matches.end(), cmd_matches.begin(), cmd_matches.end());
            }
            break;
    }
    
    // Add a space after instructions, commands, and directives if it's the only match
    if (matches.size() == 1)
    {
        if (state.context == CompletionContext::Instruction || 
            state.context == CompletionContext::Command ||
            state.context == CompletionContext::Directive)
        {
            matches[0] += " ";
        }
    }
    
    return matches;
}

/**
 * Replxx completion callback
 */
replxx::Replxx::completions_t completionCallback(std::string const& input, int& contextLen)
{
    // Replxx passes the full line, and we assume completion is requested at the end of 'input'
    int cursor_pos = static_cast<int>(input.length());
    LineState state = parseLine(input, cursor_pos);
    
    // contextLen is the number of characters to the left of the cursor to be replaced
    contextLen = cursor_pos - state.current_token.start_pos;
    
    std::vector<std::string> matches = completeLine(input, cursor_pos);
    
    return replxx::Replxx::completions_t(matches.begin(), matches.end());
}

/**
 * Initialize the completion system
 */
void initCompletion()
{
    // Nothing special to initialize for now
    // Data structures are static and ready to use
}

/**
 * Register the completion callback with replxx
 */
void registerCompletionCallback(replxx::Replxx& rx)
{
    rx.set_completion_callback(completionCallback);
}

} // namespace Mips32