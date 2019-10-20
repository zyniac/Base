declare class console {
    /**
     * Function for writing logs with prefix
     * @example
     * console.log("Text");
     * // Output: JS -> Text
     */
    static log(msg:string, ...optionalParams: string[]):void;
    /**
     * Function for writing messages without prefix
     * @example
     * console.writeLine("Text");
     * // Output: Text
     */
    static writeLine(msg:string, optionalParams: string[]):void;
    /**
     * Function for clearing console
     * @example
     * console.clear();
     */
    static clear():void;
    /**
     * Function for getting user input.
     * @example
     * const input = console.getLine("Input: ");
     * // Output: Input: _
     */
    static getLine(msg:string):string;
    /**
     * Function for getting user input
     * but in console it is censored
     * @example
     * const password = console.getLineCensored("Password: ");
     * // Output: Password: _
     */
    static getLineCensored(msg:string):string;
    /**
     * Function to write without endlines.
     * @example
     * console.write("Text");
     * // Output: Text(without \n)
     */
    static write(msg:string):void;
}
/**
 * Account Object presents an Account on Server
 * @example
 * const account = new Account();
 */
declare class Account
{
    constructor();
    constructor(pointer:string);
    /**
     * Sets the name of the account.
     * Try to avoid this function.
     * @example
     * account.setName(name);
     */
    setName(name:string):void;
    /**
     * Gets the name
     * @example
     * const nameID = account.getName();
     * // or
     * const nameID = account.name;
     */
    getName():string;
    /**
     * Queries an account.
     * @example
     * const account = new Account();
     * account.setPointer(name); // Query a user
     * if(account.valid) {} // Checks if user valid
     */
    setPointer(name:string):void;
    /**
     * Has the same functionality as getName.
     * @example
     * const name = account.getPointer();
     */
    getPointer():string;
    /**
     * Checks if account is online.
     * @example
     * const online = account.isOnline();
     * // or
     * const online = account.online;
     */
    isOnline():boolean;
    /**
     * Checks if account is registered in blogpost database.
     * @example
     * const valid = account.isValid();
     * // or
     * const valid = account.valid;
     */
    isValid():boolean;
    /**
     * Checks if string is equal to server password
     * @example
     * const isPassword = account.isPassword(password);
     */
    isPassword(password:string):boolean;
    /**
     * Returns the display name of the account
     * @example
     * const displayName = account.getDisplayName();
     * // or
     * const displayName = account.displayName;
     */
    getDisplayName():string;
    /**
     * Deletes an account if it exists
     * @example
     * if(account.valid) account.delete();
     * // account.delete always true because it gets only executed if account exists
     */
    delete():boolean;
    /**
     * Sends a message if user is online
     * @example
     * const success = sendMessage(message);
     */
    sendMessage(msg:string, ...optionalParams:string[]):boolean;
    /**
     * The name of the account. This value cannot be set.
     * Use for that setName instead.
     * @example
     * const name = account.name;
     * // and
     * account.name = name;
     */
    readonly name:string;
    /**
     * Checks if account is online. This value cannot be set.
     * @example
     * const online = account.isOnline();
     * // or
     * const online = account.online;
     */
    readonly online:boolean;
    /**
     * Checks if account is registered in blogpost database.
     * This value cannot be set.
     * @example
     * const valid = account.isValid();
     * // or
     * const valid = account.valid;
     */
    readonly valid:boolean;
    /**
     * Returns the display name of the account
     * @example
     * const displayName = account.getDisplayName();
     * // or
     * const displayName = account.displayName;
     */
    readonly displayName:string;
    /**
     * Returns all accounts that are registered.
     * @example
     * const accountArray = Account.getAll();
     * // or
     * const accountArray = Account.accounts;
     */
    static readonly accounts:Array<Account>;
    /**
     * Returns all accounts that are online.
     * @example
     * const accountsOnlineArray = Account.getAllOnlne();
     * // or
     * const accountsOnlineArray = Account.accountsOnline;
     */
    static readonly accountsOnline:Array<Account>;
    /**
     * Returns all accounts that are registered.
     * @example
     * const accountArray = Account.getAll();
     * // or
     * const accountArray = Account.accounts;
     */
    static getAccounts():Array<Account>;
    /**
     * Returns all accounts that are online.
     * @example
     * const accountsOnlineArray = Account.getAllOnlne();
     * // or
     * const accountsOnlineArray = Account.accountsOnline;
     */
    static getAccountsOnline():Array<Account>;
    /**
     * Creates a new account.
     * If the name of the account already exists the function will return false
     * @example
     * const account = Account.create(username, password);
     */
    static create(name:string, password:string):boolean|Account;
}
/**
 * Path is a string typedef. It can only be accessed by Zyniac functions.
 * @example
 * Path: string;
 */
interface Path
{
    Path: string;
}
/**
 * WebContent is a string typedef. It can only be accessed by Zyniac functions.
 */
interface WebContent
{
    WebContent: string | number;
}
/**
 * Web.Server structure. This can only build statically
 */
declare class WebServer
{
    /**
     * Close the WebServer.
     * @example
     * const success = WebServer.close();
     */
    static close():boolean;
    /**
     * Open the WebServer.
     * @example
     * const success = WebServer.open();
     */
    static open():boolean;
    /**
     * Register w-script-content on the webserver.
     * @example
     * WebServer.registerContent(content, "./plugin/example");
     * // Content is now accessible on http://domain/plugin/example if function returned true
     */
    static registerContent(webpath: string):WebContent|boolean;
    /**
     * Removes content.
     * Important! The parameter must be the same object that also got registered!
     * @example
     * WebServer.removeContent(content);
     */
    static removeContent(content:WebContent);
}
/**
 * The Web.Content call Event
 */
interface WebContentOnCallEvent
{
    setContent(content:WebContent):void;
    setMIME(mime:string):void;
}
/**
 * The Key Event Selector for the Event Listeners for Web.Content
 */
interface WebContentEventKey
{
    "call": WebContentOnCallEvent;
}
/**
 * The WebContent class. This must be an object (build with new)!
 * This object is for simulating added files for WebContents or more simply, adding WebContents with scripts.
 * In short it is also known as w-script-content or wscc (Web Script Content).
 * @example
 * const webcontent = new WebContent(); // This is only the structure. Nothing is registered with this use.
 * // Use for that WebServer.registerContent instead.
 */
declare class WebContent {
    /**
     * This function is for adding events. It works almost the same as the dom event listener.
     * @example
     * webcontent.addEventListener('call', (event) => { });
     */
    addEventListener<K extends keyof WebContentEventKey>(type: K, listener: (this: Function, ev: WebContentEventKey[K]) => any):boolean;
}