let app = document.getElementById("app");
app.width = 800;
app.height = 600;
let ctx = app.getContext("2d");
let w = null;

function make_environment(...envs) {
    return new Proxy(envs, {
        get(target, prop, receiver) {
            for (let env of envs) {
                if (env.hasOwnProperty(prop)) {
                    return env[prop];
                }
            }
            return (...arge) => {console.error("NOT IMPLEMENTED: "+prop, args)}
        }
    });
}

WebAssembly.instantiateStreaming(fetch('./wasm.wasm'), {
    "env": make_environment()
}).then(w0 => {
    w = w0;
    const buffer = w.instance.exports.memory.buffer;
    const pixels = w.instance.exports.render();
    console.log(new Uint8Array(buffer, pixels, app.width*app.height*4));
})
