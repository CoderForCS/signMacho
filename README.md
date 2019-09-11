# SignMacho
[![GitHub license](https://img.shields.io/badge/license-MIT-lightgrey.svg)](https://github.com/CoderForCS/signMacho/blob/master/README.md)

  Use sign iOS ipa. 一款用于iOS重签名的命令行工具。  


## installation
  * 手工安装
>   sudo sh ./install.sh
  * brew安装
>  brew tap coderforcs/resign && brew install resign
## Usage

    resign version 1.0 Help
      Usage: resign -i <ipa path> -c <identity name> -e <profile path> [options]
      
      where options are:
        -s show current can sign Identity;
        -i need sign ipa path, as "/Desktop/myapp/sign.ipa";
        -c sign app identity name, as "XXXXCo., Ltd.";
        -e input your mobileprovisio path,as "/Desktop/myapp/embedded.mobileprovision";
        -o output resign ipa path.

## Example
> resign -i /Desktop/myapp/sign.ipa -c "XXXXCo., Ltd." -e /Desktop/myapp/embedded.mobileprovision -o /Desktop
