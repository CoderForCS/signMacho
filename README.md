# SignMacho
  Use sign iOS ipa. 一款用于iOS重签名的命令行工具。

## install
  手工安装  install
>   sh ./install.sh

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
