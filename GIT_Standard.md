```
git checkout master //切换分支
git rm -r . //删除分支全部内容
git commit -m "REMOVE ALL"

# 在 master 分支上创建并上传一个文件
echo "This is a file in master branch" > master_file.txt
git add master_file.txt
git commit -m "Add a file to master branch"
git push origin master

git checkout git
git rm -r .
git commit -m "Remove all files from git branch"

# 在 git 分支上创建并上传一个文件
echo "This is a file in git branch" > git_file.txt
git add git_file.txt
git commit -m "Add a file to git branch"
git push origin git
```

