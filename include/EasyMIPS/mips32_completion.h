#ifndef __MIPS32_COMPLETION_H__
#define __MIPS32_COMPLETION_H__

#include <string>
#include <vector>
#include <cstring>
#include <replxx.hxx>

namespace Mips32
{

/**
 * Context types for autocompletion
 */
enum class CompletionContext
{
    Instruction,    // At the start of a line or after a label - suggest instructions
    Register,       // After a '$' or in a position expecting a register
    Label,          // After a branch/jump instruction - suggest labels
    Directive,      // After a '.' - suggest assembler directives
    Command,        // After a '#' - suggest debugger commands
    Keyword,        // Special keywords for #show and #set commands
    Unknown         // Cannot determine context
};

/**
 * Token information extracted from the current input line
 */
struct TokenInfo
{
    std::string text;           // The token text
    bool is_complete;           // Is this a complete token (followed by space)
    int start_pos;              // Start position in the line
    int end_pos;                // End position (cursor position within token)
};

/**
 * Parsed line state for completion
 */
struct LineState
{
    std::string line;           // The full input line
    int cursor_pos;             // Current cursor position
    std::vector<std::string> tokens;  // All complete tokens before cursor
    TokenInfo current_token;    // The token being edited (if any)
    CompletionContext context;  // Determined completion context
};

/**
 * Completion callback type for replxx
 */
replxx::Replxx::completions_t completionCallback(std::string const& input, int& contextLen);

/**
 * Main completion function - analyzes input and provides completions
 * 
 * @param line The input line
 * @param cursor_pos Current cursor position
 * @return Vector of matching completions
 */
std::vector<std::string> completeLine(const std::string &line, int cursor_pos);

/**
 * Parse the input line to determine completion context
 * 
 * @param line The input line
 * @param cursor_pos Current cursor position
 * @return LineState with parsed information
 */
LineState parseLine(const std::string &line, int cursor_pos);

/**
 * Get completion suggestions for instructions
 * 
 * @param prefix The partial instruction name
 * @return Vector of matching instruction names
 */
std::vector<std::string> completeInstruction(const std::string &prefix);

/**
 * Get completion suggestions for registers
 * 
 * @param prefix The partial register name (may include '$')
 * @return Vector of matching register names (with '$' prefix)
 */
std::vector<std::string> completeRegister(const std::string &prefix);

/**
 * Get completion suggestions for assembler directives
 * 
 * @param prefix The partial directive name (may include '.')
 * @return Vector of matching directive names (with '.' prefix)
 */
std::vector<std::string> completeDirective(const std::string &prefix);

/**
 * Initialize the completion system
 * Call this before using the completion functions
 */
void initCompletion();

/**
 * Register completion callback with replxx
 */
void registerCompletionCallback(replxx::Replxx& rx);

} // namespace Mips32

#endif // __MIPS32_COMPLETION_H__