/* vm/memory/error.hpp */

void report_assertion_failure(char* code_str, char* file_name, int line_no, char* message) {
    __coverity_panic__();
}
void report_fatal(char* file_name, int line_no, char* format, ...) {
    __coverity_panic__();
}
void report_should_not_call(char* file_name, int line_no) {
    __coverity_panic__();
}
void report_should_not_reach_here(char* file_name, int line_no) {
    __coverity_panic__();
}
void report_subclass_responsibility(char* file_name, int line_no) {
    __coverity_panic__();
}
void report_unimplemented(char* file_name, int line_no) {
    __coverity_panic__();
}