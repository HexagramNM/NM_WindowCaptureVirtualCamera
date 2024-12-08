@echo off

cd External\windows-classic-samples

git config core.sparsecheckout true

cd ..\..

echo Samples/Win7Samples/multimedia/directshow/baseclasses > .git\modules\External\windows-classic-samples\info\sparse-checkout

git -C External/windows-classic-samples read-tree -mu HEAD