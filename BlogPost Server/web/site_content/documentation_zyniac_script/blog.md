# Zyniac Script Documentation

This is the official Zyniac documentation for jsz (Zyniac Javascript) compiled files.

<br>

- [Zyniac Script Documentation](#zyniacscriptdocumentation)
  - [Basic introduction to jsz files](#basicintroductiontojszfiles)
  - [Why should I use ZS?](#whyshouldiusezs)
  - [Begin coding](#begincoding)
- [Documentation](#documentation)
  - [Account](#account)
    - [What is an account id](#whatisanaccountid)
  - [Account Object Functions](#accountobjectfunctions)
    - [account.setName](#accountsetname)
    - [account.getName](#accountgetname)
    - [account.setPointer](#accountsetpointer)
    - [account.getPointer](#accountgetpointer)
    - [account.isValid](#accountisvalid)
    - [account.isOnline](#accountisonline)
    - [account.isPassword](#accountispassword)
    - [account.delete](#accountdelete)
    - [account.getDisplayName](#accountgetdisplayname)
    - [account.sendMessage](#accountsendmessage)
  - [Account Object Variables](#account-object-variables)
    - [account.name](#accountname)
    - [account.pointer](#accountpointer)
    - [account.displayName](#accountdisplayname)
    - [account.valid](#accountvalid)
    - [account.online](#accountonline)
  - [Account Static Functions](#account-static-functions)
    - [Account.getAll](#accountgetall)
    - [Account.getAllOnline](#accountgetallonline)
  - [Account Static Variables](#account-static-variables)
    - [Account.accounts](#accountaccounts)
    - [Account.accountsOnline](#accountaccountsonline)
<br>

## Basic introduction to jsz files

For what are Zyniac scripts made? Easy answer. Javascript is developed for front-end web developement stuff. With ZS (Zyniac Script) **you manage data in the backend section** were everything is processed. Firstly, jsz files will not be something new to you if you already worked with javascript. ZS is built in the same language but with different libraries. The default libraries like ``JSON``, ``Math``, ``Date`` and so on are also in the Blogpost package, but there is a lot more than only this few objects.

<br>

> Like you can see, the basics are the same so I won't mention the syntax of javascript in this tutorial.

## Why should I use ZS?

This is the easiest and fastest way to create and customize a webserver, the features are limitless because every person can upload its own plugin, the API is very powerful and the server uses as few performance as possible (developed in C++). There are also some services like shared command system and jszweb what helps you even more customize your server. Quickly said, you have access to everything exept private data. This exists only internal but you can control it with a handle in javascript.
<br>

## Begin coding
Download [this file](https://google.de), and add it to your directory. Add this line *on top of your Zyniac script*. This should make your editor intellisence better:

``/// <reference path="lib.zyniac.d.ts" />``

# Documentation

## Account

An account object is *a reference* to an account in Zyniac Server. You can construct it like a normal object.

```js
const account = new Account();
```

To reference an account object to a real account use the constructor parameter or the ``setPointer`` function. This will require an id as string type. 

```js
const account = new Account("id");
// or
account.setPointer("id");
```

### What is an account id

An account id is only the accounts name with lower case characters. If you put in uppercase characters, they will get converted.

## Account Object Functions

### account.setName

``setName()`` changes the account name. Note that the id will also change. Zyniac automatically changes the account reference to the renamed account. If a user is online, he will stay logged in. The function returns a boolean. If it is true the account changed its name successfully, else something went wrong like the username already exists.

```js
if(account.setName(newName))
{
    console.writeLine("Account successfully changed.");
}
else
{
    console.writeLine("Account name already exists");
}
```

### account.getName

``getName()`` gets the account name. It does not return the display name but the name id. You need no parameters for this function.

```js
const name = account.getName();
```

### account.setPointer

``setPointer()`` sets the reference to an account as stated above.

```js
account.setPointer("id");
```

### account.getPointer

``getPointer()`` returns the same value as getName because it should always be the same value if looked up to the account type or read locally. If you want to know if user exists use another function.

```js
const name = account.getPointer();
```

### account.isValid

``isValid()`` returns a boolean. It is true if account exists, else it is false.

```js
const exists = account.isValid();
```

### account.isOnline

``isOnline()`` returns also a boolean. It is also true if account exists, else it is also false.

```js
const online = account.isOnline();
```

### account.isPassword

``isPassword()`` checks if password is valid. It requires the possible password (string) as parameter and returns true if password exists, else false.

```js
if(account.isPassword(password))
{
    console.writeLine("This is the password");
}
else
{
    console.writeLine("Password not valid.");
}
```

### account.delete

``delete()`` delete the selected account. It returns a boolean if the function had success.

```js
if(account.delete())
{
    console.writeLine("Account erased.");
}
else
{
    console.writeLine("Account could not be erased.");
}
```

### account.getDisplayName

``getDisplayName()`` returns the string which the server would output to a user as account name.

```js
const displayName = account.getDisplayName();
```

### account.sendMessage

``sendMessage()`` sends message packets to a client which is logged in with this account. If a user is offline this function would return false.

```js
account.sendMessage(message);
```

## Account Object Variables

**Most Zyniac variables can be replaced by a function.** If you use a variable you'll not really call such, its more another notation for calling the same function without return value when setting it.

### account.name

- [x] Only for reading

``account.name`` returns the nameID like account.getName and ``account.getPointer``

```js
const name = account.name;
```

### account.pointer

``account.pointer`` is for setting and getting the pointer like ``account.setPointer`` and account.getPointer. It will do the same because ``account.setPointer`` also returns no value. Read [this](#account) if you don't know what a *pointer* is.


```js
const name = account.pointer;
// or
account.pointer = name;
```

### account.displayName

- [x] Only for reading

```js
const displayName = account.displayName;
```

### account.valid

- [x] Only for reading

Checks if account is valid.

```js
if(account.valid)
{
    console.writeLine("Account is valid");
}
```

### account.online

- [x] Only for reading

Checks if account is online.

```js
if(account.online)
{
    console.writeLine("Account is online");
}
```

## Account Static Functions

This are the functions where you don't need to initiate an account object for. I will try to explain it to you with an example. This isn't so difficult actually.

```js
const account = new Account(); // Initiation, we don't need this
// -------------------
const accounts = Account.getAccounts();
// Made account objects without initiation (using static function)
```

### Account.getAccounts

``getAccounts`` fetches all Accounts from server into a list. You shouldn't use this if you have many accounts.

```js
const accounts = Account.getAccounts();
```

### Account.getAccountsOnline

``getAccountsOnline`` fetches all Accounts which are online from the server into a list. You also shouldn't use this if you have many accounts.

```js
const accounts = Account.getAccountsOnline();
```

## Account Static Variables

- [x] Only for reading

This variables are equivalent with the functions. It's not important wich variant you are using.

### Account.accounts

Same as ``getAccounts``

```js
const accounts = Account.accounts;
```

### Account.accountsOnline

Same as ``getAccountsOnline``

```js
const accounts = Account.accountsOnline;
```