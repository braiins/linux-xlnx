#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import getpass

def GetCmdRst(cmd):
    """
    返回cmd的第一行值
    """
    cmd_file = os.popen(cmd)
    rst = cmd_file.readline()
    rst = rst.strip('\n')
    cmd_file.close()
    return rst

if __name__ == '__main__':
    version_file_name = sys.argv[1]

    # 编译用户
    build_user = GetCmdRst('whoami')

    # 编译机器
    build_server = GetCmdRst('hostname -I')

    # 编译机器
    commit_info = GetCmdRst(r"git show --pretty=format:'%H,%cn,%ce,%ai' | head -n 1")
    commit_info = commit_info.split(',')
    commit_hash = commit_info[0]
    commit_user = commit_info[1]
    commit_user_email = commit_info[2]
    commit_time = commit_info[3]

    msg_tuple = (build_user, build_server, commit_hash, commit_user, commit_user_email, commit_time)
    #print msg_tuple

    # 写入文件
    version_fmt = """/* auto generated file, do not modify. */

#ifndef _INNO_VERSION_H_
#define _INNO_VERSION_H_

#define INNO_VERSION_BUILD_USER           ("%s")
#define INNO_VERSION_BUILD_SERVER         ("%s")
#define INNO_VERSION_COMMIT_HASH          ("%s")
#define INNO_VERSION_COMMIT_USER          ("%s")
#define INNO_VERSION_COMMIT_USER_EMAIL    ("%s")
#define INNO_VERSION_COMMIT_TIME          ("%s")

#endif
"""
    version_file = open(version_file_name, 'w')
    version_content = version_fmt % msg_tuple
    version_file.write(version_content)
    version_file.close()

