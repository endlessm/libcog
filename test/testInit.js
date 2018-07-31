const {Cog} = imports.gi;

describe('Initialization', function () {
    it('works', function () {
        expect(Cog.is_inited()).toBeFalsy();
        Cog.init_default();
        expect(Cog.is_inited()).toBeTruthy();
        Cog.shutdown();
    });
});
