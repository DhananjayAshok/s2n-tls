3c --addcr --alltypes --warn-root-cause --output-dir=out -p=build/compile_commands.json error/s2n_errno.c \
stuffer/s2n_stuffer_base64.c \
stuffer/s2n_stuffer_file.c \
stuffer/s2n_stuffer_network_order.c \
stuffer/s2n_stuffer_pem.c \
stuffer/s2n_stuffer_text.c \
stuffer/s2n_stuffer.c \
utils/s2n_safety.c \
utils/s2n_blob.c \
utils/s2n_mem.c \
--wildptrstats-output=checked-c-scripts/3c_1_wildptrstats.json \
--perptrstats-output=checked-c-scripts/3c_1_perptrstats.json \
 --constraint-output=checked-c-scripts/3c_1_constraint.json \
 --stats-output=checked-c-scripts/3c_1_stats.json \
--dump-stats 
