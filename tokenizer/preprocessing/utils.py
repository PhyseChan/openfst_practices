"""
Author: Himiko
Date: 2022-06-29 19:30:24
LastEditors: Himiko
LastEditTime: 2022-10-09 19:45:13
"""

import gc
import pickle  # nosec:B403


def data2pkl(filename, data):
    filename = filename[:-4] if filename[-4:] == ".pkl" else filename
    with open(filename + ".pkl", "wb") as fpkl:
        pickle.dump(data, fpkl, -1)
    return None


def pkl2data(filename):
    # disable garbage collector
    gc.disable()
    filename = filename[:-4] if filename[-4:] == ".pkl" else filename
    with open(filename + ".pkl", "rb") as fdata:
        data = pickle.load(fdata)  # nosec
    # enable garbage collector again
    gc.enable()
    return data


def merge_twoDicts_setType(dict1, dict2):
    # merge
    dicts = {**dict1, **dict2}
    # get same keys
    duplicate_keys = dict1.keys() & dict2.keys()
    for key in duplicate_keys:
        # if same, skip
        if dict1[key] == dict2[key]:
            continue
        # combine them
        dicts[key] = list(set(dict1[key]).union(set(dict2[key])))
    return dicts


# update the nvn relationship
def update_nvn(words, elems, matches):

    # # todo：remove colon
    # words = "_".join([word.split(":")[0] for word in words.split("_")])

    # add match
    def add_match(dict1, words, elems):
        useless = False
        if dict1.get(words):
            if elems not in dict1[words]:
                dict1[words] += [elems]
            else:
                useless = True
        else:
            dict1[words] = [elems]
        return useless

    length = len(words.split("_"))
    if length == 2:
        # na_vb
        u = add_match(matches, words, elems)

        if u:
            print("Attention!!", words, elems, "already exists!!")

    elif length == 3:
        w1, w2, w3 = words.split("_")
        e1, e2, e3 = elems.split("_")
        # na_vb_nb1
        u = add_match(matches, words, elems)
        u1 = add_match(matches, "%s_%s" % (w1, w2), "%s_%s" % (e1, e2))
        u2 = add_match(matches, "%s_%s" % (w1, w3), "%s_%s" % (e1, e3))
        u3 = add_match(matches, "%s_%s" % (w2, w3), "%s_%s" % (e2, e3))

        if u and u1 and u2 and u3:
            print("Attention!!", words, elems, "already exists!!")

    elif length == 4:
        w1, w2, w3, w4 = words.split("_")
        e1, e2, e3, e4 = elems.split("_")
        # na_vb_nb1_nb2
        u = add_match(matches, words, elems)
        # na_vb_nb1
        u1 = add_match(matches, "%s_%s_%s" % (w1, w2, w3), "%s_%s_%s" % (e1, e2, e3))
        u11 = add_match(matches, "%s_%s" % (w1, w2), "%s_%s" % (e1, e2))
        u12 = add_match(matches, "%s_%s" % (w2, w3), "%s_%s" % (e2, e3))
        u13 = add_match(matches, "%s_%s" % (w1, w3), "%s_%s" % (e1, e3))
        # na_vb_nb2
        u2 = add_match(matches, "%s_%s_%s" % (w1, w2, w4), "%s_%s_%s" % (e1, e2, e4))
        u22 = add_match(matches, "%s_%s" % (w2, w4), "%s_%s" % (e2, e4))
        u23 = add_match(matches, "%s_%s" % (w1, w4), "%s_%s" % (e1, e4))
        # vb_nb1_nb2
        u3 = add_match(matches, "%s_%s_%s" % (w2, w3, w4), "%s_%s_%s" % (e2, e3, e4))

        if u and u1 and u11 and u12 and u13 and u2 and u22 and u23 and u3:
            print("Attention!!", words, elems, "already exists!!")

    return matches


# update the nvn-including-pN relationship
def update_nvn_pN(words, elems, matches):
    # add match
    def add_match(dict1, words, elems):
        useless = False
        if dict1.get(words):
            if elems not in dict1[words]:
                dict1[words] += [elems]
            else:
                useless = True
        else:
            dict1[words] = [elems]
        return useless

    length = len(words.split("_"))
    if length == 3:
        w1, w2, wp = words.split("_")
        e1, e2, ep = elems.split("_")
        # general
        u2 = add_match(matches, "%s_%s" % (w1, w2), "%s_%s" % (e1, e2))
        u3 = add_match(matches, "%s_%s" % (wp.split("+")[0], w2), "prep_V")

        # p_na_v
        if elems in [
            "na_vab_p1Nb1",
            "na_va_p1Nb1",
            "na_vb_p1Nb1",
        ]:
            u = add_match(matches, "%s_%s_%s" % (wp, w1, w2), "%s_%s_%s" % (ep, e1, e2))
            u4 = add_match(matches, "%s_%s" % (wp, w2), "%s_%s" % (ep, e2))
            if u and u2 and u3 and u4:
                print("Attention!!", words, elems, "already exists!!")
        # na_p_v
        elif elems in [
            "na_vab_p2Na",
            "na_va_p2Na",
            "na_vb_p2Na",
            "na_vab_p2Nb1",
            "na_va_p2Nb1",
            "na_vb_p2Nb1",
        ]:
            u = add_match(matches, "%s_%s_%s" % (w1, wp, w2), "%s_%s_%s" % (e1, ep, e2))
            u4 = add_match(matches, "%s_%s" % (wp, w2), "%s_%s" % (ep, e2))
            if u and u2 and u3 and u4:
                print("Attention!!", words, elems, "already exists!!")
        # na_v_p
        elif elems in [
            "na_vab_p4Nb1",
            "na_va_p4Nb1",
            "na_vb_p4Nb1",
        ]:
            u = add_match(matches, "%s_%s_%s" % (w1, w2, wp), "%s_%s_%s" % (e1, e2, ep))
            u4 = add_match(matches, "%s_%s" % (w2, wp), "%s_%s" % (e2, ep))
            if u and u2 and u3 and u4:
                print("Attention!!", words, elems, "already exists!!")

    elif length == 4:
        # class 1
        if elems in [
            "na_va_nb2_p1Nb1",
            "na_vb_nb2_p1Nb1",
            "na_va_nb1_p2Na",
            "na_vb_nb1_p2Na",
            "na_va_nb1_p2Nb2",
            "na_vb_nb1_p2Nb2",
            "na_va_nb2_p2Nb1",
            "na_vb_nb2_p2Nb1",
            "na_va_nb2_p4Nb1",
            "na_vb_nb2_p4Nb1",
            "na_va_nb2_p5Nb1",
            "na_vb_nb2_p5Nb1",
        ]:
            w1, w2, w3, wp = words.split("_")
            e1, e2, e3, ep = elems.split("_")
            u1 = add_match(
                matches, "%s_%s_%s" % (w1, w2, w3), "%s_%s_%s" % (e1, e2, e3)
            )
            u11 = add_match(matches, "%s_%s" % (w1, w2), "%s_%s" % (e1, e2))
            u12 = add_match(matches, "%s_%s" % (w1, w3), "%s_%s" % (e1, e3))
            u13 = add_match(matches, "%s_%s" % (w2, w3), "%s_%s" % (e2, e3))
            u3 = add_match(matches, "%s_%s" % (wp.split("+")[0], w2), "prep_V")
            # p_na_v_nb1
            if elems in [
                "na_va_nb2_p1Nb1",
                "na_vb_nb2_p1Nb1",
            ]:
                u = add_match(
                    matches,
                    "%s_%s_%s_%s" % (wp, w1, w2, w3),
                    "%s_%s_%s_%s" % (ep, e1, e2, e3),
                )
                u2 = add_match(
                    matches, "%s_%s_%s" % (wp, w1, w2), "%s_%s_%s" % (ep, e1, e2)
                )
                u4 = add_match(
                    matches, "%s_%s_%s" % (wp, w2, w3), "%s_%s_%s" % (ep, e2, e3)
                )
                u5 = add_match(matches, "%s_%s" % (wp, w2), "%s_%s" % (ep, e2))
                if u and u1 and u11 and u12 and u13 and u2 and u3 and u4 and u5:
                    print("Attention!!", words, elems, "already exists!!")

            # na_p_v_nb1
            elif elems in [
                "na_va_nb1_p2Na",
                "na_vb_nb1_p2Na",
                "na_va_nb1_p2Nb2",
                "na_vb_nb1_p2Nb2",
                "na_va_nb2_p2Nb1",
                "na_vb_nb2_p2Nb1",
            ]:
                u = add_match(
                    matches,
                    "%s_%s_%s_%s" % (w1, wp, w2, w3),
                    "%s_%s_%s_%s" % (e1, ep, e2, e3),
                )
                u2 = add_match(
                    matches, "%s_%s_%s" % (w1, wp, w2), "%s_%s_%s" % (e1, ep, e2)
                )
                u4 = add_match(
                    matches, "%s_%s_%s" % (wp, w2, w3), "%s_%s_%s" % (ep, e2, e3)
                )
                u5 = add_match(matches, "%s_%s" % (wp, w2), "%s_%s" % (ep, e2))
                if u and u1 and u11 and u12 and u13 and u2 and u3 and u4 and u5:
                    print("Attention!!", words, elems, "already exists!!")

            # na_v_p_nb2
            elif elems in [
                "na_va_nb2_p4Nb1",
                "na_vb_nb2_p4Nb1",
            ]:
                u = add_match(
                    matches,
                    "%s_%s_%s_%s" % (w1, w2, wp, w3),
                    "%s_%s_%s_%s" % (e1, e2, ep, e3),
                )
                u2 = add_match(
                    matches, "%s_%s_%s" % (w1, w2, wp), "%s_%s_%s" % (e1, e2, ep)
                )
                u4 = add_match(
                    matches, "%s_%s_%s" % (w2, wp, w3), "%s_%s_%s" % (e2, ep, e3)
                )
                u5 = add_match(matches, "%s_%s" % (w2, wp), "%s_%s" % (e2, ep))
                if u and u1 and u11 and u12 and u13 and u2 and u3 and u4 and u5:
                    print("Attention!!", words, elems, "already exists!!")

            # na_v_nb2_p
            elif elems in [
                "na_va_nb2_p5Nb1",
                "na_vb_nb2_p5Nb1",
            ]:
                u = add_match(
                    matches,
                    "%s_%s_%s_%s" % (w1, w2, w3, wp),
                    "%s_%s_%s_%s" % (e1, e2, e3, ep),
                )
                u2 = add_match(
                    matches, "%s_%s_%s" % (w1, w2, wp), "%s_%s_%s" % (e1, e2, ep)
                )
                u4 = add_match(
                    matches, "%s_%s_%s" % (w2, w3, wp), "%s_%s_%s" % (e2, e3, ep)
                )
                u5 = add_match(matches, "%s_%s" % (w2, wp), "%s_%s" % (e2, ep))
                if u and u1 and u11 and u12 and u13 and u2 and u3 and u4 and u5:
                    print("Attention!!", words, elems, "already exists!!")

        # class 2
        elif elems in [
            "na_va_p2Nb2_p3Nb1",
            "na_vb_p2Nb2_p3Nb1",
            "na_va_p2Nb2_p4Nb1",
            "na_vb_p2Nb2_p4Nb1",
        ]:
            w1, w2, wp1, wp2 = words.split("_")
            e1, e2, ep1, ep2 = elems.split("_")
            u11 = add_match(
                matches, "%s_%s_%s" % (w1, wp1, w2), "%s_%s_%s" % (e1, ep1, e2)
            )
            u31 = add_match(matches, "%s_%s" % (wp1, w2), "%s_%s" % (ep1, e2))
            u41 = add_match(matches, "%s_%s" % (wp1.split("+")[0], w2), "prep_V")
            u42 = add_match(matches, "%s_%s" % (wp2.split("+")[0], w2), "prep_V")
            if elems in [
                "na_va_p2Nb2_p3Nb1",
                "na_vb_p2Nb2_p3Nb1",
            ]:
                u = add_match(
                    matches,
                    "%s_%s_%s_%s" % (w1, wp1, wp2, w2),
                    "%s_%s_%s_%s" % (e1, ep1, ep2, e2),
                )
                u12 = add_match(
                    matches, "%s_%s_%s" % (w1, wp2, w2), "%s_%s_%s" % (e1, ep2, e2)
                )
                u2 = add_match(
                    matches, "%s_%s_%s" % (wp1, wp2, w2), "%s_%s_%s" % (ep1, ep2, e2)
                )
                u32 = add_match(matches, "%s_%s" % (wp2, w2), "%s_%s" % (ep2, e2))
                if u and u11 and u12 and u2 and u31 and u32 and u41 and u42:
                    print("Attention!!", words, elems, "already exists!!")

            elif elems in [
                "na_va_p2Nb2_p4Nb1",
                "na_vb_p2Nb2_p4Nb1",
            ]:
                u = add_match(
                    matches,
                    "%s_%s_%s_%s" % (w1, wp1, w2, wp2),
                    "%s_%s_%s_%s" % (e1, ep1, e2, ep2),
                )
                u12 = add_match(
                    matches, "%s_%s_%s" % (w1, w2, wp2), "%s_%s_%s" % (e1, e2, ep2)
                )
                u2 = add_match(
                    matches, "%s_%s_%s" % (wp1, w2, wp2), "%s_%s_%s" % (ep1, e2, ep2)
                )
                u32 = add_match(matches, "%s_%s" % (w2, wp2), "%s_%s" % (e2, ep2))
                if u and u11 and u12 and u2 and u31 and u32 and u41 and u42:
                    print("Attention!!", words, elems, "already exists!!")

    return matches
