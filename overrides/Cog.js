/* exported _init */

function promisify(prototype, asyncName, finishName) {
    prototype[`_real_${asyncName}`] = prototype[asyncName];
    prototype[asyncName] = function(...args) {
        if (!args.every(arg => typeof arg !== 'function'))
            return this[`_real_${asyncName}`](...args);

        return new Promise((resolve, reject) => {
            const callerStack = new Error().stack
                .split('\n')
                .filter(line => !line.match(/promisify/))
                .join('\n');
            this[`_real_${asyncName}`](...args, function(source, res) {
                try {
                    const result = source[finishName](res);
                    resolve(result);
                } catch (error) {
                    if (error.stack)
                        error.stack += `--- Called from: ---\n${callerStack}`;
                    else
                        error.stack = callerStack;
                    reject(error);
                }
            });
        });
    };
}

function _init() {
    const Cog = this;

    promisify(Cog.Client.prototype, 'get_user_async', 'get_user_finish');
    promisify(Cog.Client.prototype, 'initiate_auth_async',
        'initiate_auth_finish');
    promisify(Cog.Client.prototype, 'sign_up_async', 'sign_up_finish');
    promisify(Cog.Client.prototype, 'update_user_attributes_async',
        'update_user_attributes_finish');
}
