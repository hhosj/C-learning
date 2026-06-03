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

WebAssembly.instantiateStreaming(fetch('./wasm.o'), {
    "env": make_environment()
}).then(w0 => {
    w = w0;
    console.log(w);
})
