const {Cog} = imports.gi;

describe('API client', function () {
    beforeAll(function () {
        Cog.init_default();
    });

    it('can be constructed', function () {
        void new Cog.Client();
    });
});
