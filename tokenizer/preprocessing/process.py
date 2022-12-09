import utils

output = utils.pkl2data('output_mcd.pkl')
mcd_dict = output['mcd']
with open("mcd.txt", "a", encoding="utf-8") as f:
    for idx, k in enumerate(mcd_dict.keys()):
        f.write(str(k)+'\t'+str(idx+1)+'\n')
    f.flush()
    f.close()

with open("char.txt", "a", encoding="utf-8") as f_c:
    idx = 1
    char_set = set()
    for k in mcd_dict.keys():
        c_list = []
        for c in k:
            c_list.append(c)
        char_set = char_set.union(set(c_list))
    for item in char_set:
        f_c.write(item+'\t'+str(idx)+'\n')
        idx += 1
    f_c.flush()
    f_c.close()
    