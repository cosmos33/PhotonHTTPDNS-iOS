#!/usr/bin/env
#encoding:utf-8
import argparse
import sys;
import re;
import os;
#接受参数的输入

#工程的当前目录
base_path = '';
#tag所在的目录
tag_path = '../MMSpecs/'

def renoveTag(name,tag):
    
    pass

def writeNewTagToSpecFile(name,tag,am):
    tagStr = "'"+ tag+ "'";
    result = '';
    place_line = '';
    f = open(base_path + name + '.podspec','r+',0);
    for line in f.readlines():
        match1 = re.match(r'\s*\w*.version \s*=\s*\'.*\'',line);
        print match1;
        if match1 :
            place_line = line;
            result = re.sub(r'\'.*\'',tagStr,line,count=0, flags=0)
        else:
            continue;
    f.close();

    f = open(base_path + name + '.podspec', 'r', 0);
    content = f.read();
    result1 = re.sub(place_line, result, content, count=0, flags=0)
    f.close();
    if result1:
        f = open(base_path + name + '.podspec', 'w+', 0);
        f.write(result1);
        f.close();
        print('git commit -am"'+am+'"');
        os.system('git commit -am"'+am+'"');
        os.system('git pull');
        os.system('git push');
        print('git tag -d '+tag);
        os.system('git tag -d '+tag);
        print('git push origin :'+tag);
        os.system('git push origin :'+tag);
        print('git tag '+tag+' -m"'+am+'"');
        os.system('git tag '+tag+' -m"'+am+'"');
        os.system('git push --tags');
        path = tag_path + name + '/' + tag;
        print path;
        spec_name = name + '.podspec';
        os.system('mkdir '+ path);
        os.system('cp -r '+ spec_name + ' ' + path + '/'+spec_name);
        print tag_path + name;
        os.chdir(''+ tag_path + name);
        os.system('git status');
        os.system('git add *');
        os.system('git status');
        os.system('git commit -am"'+am+'"');
        os.system('git status');
        os.system('git pull');
        os.system('git status');
        os.system('git push');
        os.system('git status');


# main方法
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='');
    parser.add_argument('--name',type=str,default='PhotonHTTPDNS');
    parser.add_argument('--tag',type=str,default=None);
    parser.add_argument('--am',type=str,default=None);
    args = parser.parse_args();
    writeNewTagToSpecFile(args.name,args.tag,args.am);
