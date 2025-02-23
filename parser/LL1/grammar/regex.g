regex	= conc regex_post

regex_post = '|' regex | null

conc	= qual_quant conc_post
conc_post = conc | null

qual_quant	= qual quant

quant	= '+' | '*' | '?' | quant_range | null
quant_range	= '{' quant_core '}'
quant_core = num ',' quant_core_post | ',' num
quant_core_post = num | null

qual = '.' | char | '\' char | '(' regex ')' | '[' qual_range ']'
qual_range = char qual_range_post
qual_range_post = '-' char qual_range | qual_range | null


char = RANGE(char(32) - char(126))
num = RANGE('1' - '9') num_tail 
num_tail = RANGE('0' - '9') num_tail | null