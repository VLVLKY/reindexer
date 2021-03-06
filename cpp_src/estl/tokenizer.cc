#include "tokenizer.h"
#include <stdlib.h>

namespace reindexer {

static inline bool isalpha(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
static inline bool isdigit(char c) { return (c >= '0' && c <= '9'); }
static inline char tolower(char c) { return (c >= 'A' && c <= 'Z') ? c + 'a' - 'A' : c; }

tokenizer::tokenizer(const string &query) : cur(query.c_str()) {}

bool tokenizer::end() const { return !*cur; }

void tokenizer::skip_space() {
	while (*cur == ' ' || *cur == '\t') cur++;
}

token tokenizer::next_token(bool to_lower) {
	skip_space();

	if (!*cur) return token(TokenEnd, "");

	token res;
	res.type = TokenSymbol;

	if (isalpha(*cur)) {
		res.type = TokenName;
		do {
			res.text += to_lower ? tolower(*cur++) : *cur++;
		} while (isalpha(*cur) || isdigit(*cur) || *cur == '_');
	} else if (isdigit(*cur) || *cur == '-' || *cur == '+') {
		res.type = TokenNumber;
		do {
			res.text += *cur++;
		} while (isdigit(*cur));
	} else if (*cur == '>' || *cur == '<' || *cur == '=') {
		res.type = TokenOp;
		do {
			res.text += *cur++;
		} while ((*cur == '=' || *cur == '>' || *cur == '<') && res.text.length() < 2);
	} else if (*cur == '"' || *cur == '\'' || *cur == '`') {
		res.type = TokenString;
		char quote_chr = *cur++;
		while (*cur) {
			if (*cur == quote_chr) {
				++cur;
				break;
			}
			if (*cur == '\\' && !*++cur) break;
			res.text += *cur++;
		};
	} else
		res.text = *cur++;

	//	printf("tok=%s\n", res.text.c_str());
	return res;
}

token tokenizer::peek_token(bool to_lower) {
	auto save_cur = cur;
	auto res = next_token(to_lower);
	cur = save_cur;
	return res;
}

}  // namespace reindexer
