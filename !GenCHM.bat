copy /y Readme.html docs\html\index.html
copy /y doxygen.css docs\html\doxygen.css
cd docs\html
"G:\Program Files\HTML Help Workshop\hhc.exe" index.hhp
cd ..\..
