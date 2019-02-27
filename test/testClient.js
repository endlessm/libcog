const {Cog} = imports.gi;

describe('API client', function () {
    beforeAll(function () {
        Cog.init_default();
    });

    it('can be constructed', function () {
        void new Cog.Client();
    });

    it('can be constructed with params', function () {
        void new Cog.Client({region: Cog.Region.US_EAST_2});
    });
});
