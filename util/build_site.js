// Node.js script for building the website.

const path = require('node:path');
const fs = require('fs');
const Handlebars = require("handlebars");
const showdown  = require('showdown');
const { readFileSync } = require('node:fs');
const converter = new showdown.Converter({metadata: true});
const Prism = require('prismjs');
const loadLanguages = require('prismjs/components/');
loadLanguages(['cpp']);

// TODO: Do the whole let, var, const proper.

// TODO: Make cmd line params and have this script be relocatable.
let docs_dir = path.join(__dirname, "..", "docs");
let ppl_dir = path.join(docs_dir, "PPL");
let build_dir = path.join(__dirname, "..", "_site");
let static_dir = path.join(ppl_dir, "static");

// create build dir.
if (!fs.existsSync(build_dir)) {
    fs.mkdirSync(build_dir);
}

// clear build dir.
for (let file of fs.readdirSync(build_dir)) {
    let file_path = path.join(build_dir, file);
    // ensure is a file path
    if (file_path.includes(".")) {
        // https://nodejs.org/api/fs.html#fspromisesunlinkpath
        fs.unlinkSync(file_path)
    }
}


fs.copyFileSync(path.join(static_dir, "styles.css"), path.join(build_dir, "styles.css"));
fs.copyFileSync(path.join(static_dir, "prism.css"), path.join(build_dir, "prism.css"));
//fs.copyFileSync(path.join(static_dir, "prism.js"), path.join(build_dir, "prism.js"));
fs.copyFileSync(path.join(static_dir, "PPL.png"), path.join(build_dir, "PPL.png"));

// build .html files from the .c files
let c_template = Handlebars.compile(readFileSync(path.join(static_dir, "template.html"), "utf8"));

let index_template = Handlebars.compile(readFileSync(path.join(static_dir, "index_template.html"), "utf8"));

let fileNames = [];

for (let file of fs.readdirSync(ppl_dir)) {
    if (file.endsWith(".c") || file.endsWith(".cpp")) {
        let file_extension = file.split(".").pop();
        let file_build_name = file.replace("." + file_extension, ".html");
        fileNames.push({
            file,
            file_build_name
        });
    }
}

for (let i = 0; i < fileNames.length; i++) {    
    let before_file = i == 0 ? {file_build_name: ""} : fileNames[i - 1];
    let after_file = i == fileNames.length - 1 ? {file_build_name: ""} : fileNames[i + 1];

    let file_src_path = path.join(ppl_dir, fileNames[i].file);    
    let file_build_path = path.join(build_dir, fileNames[i].file_build_name);
    let file_raw = fs.readFileSync(file_src_path, "utf8");
    
    // Returns a highlighted HTML string
    const html_styled = Prism.highlight(file_raw, Prism.languages.cpp, 'cpp');
    
    let file_compiled = c_template({
        content: html_styled,
        before_link: `<a href=\" ${before_file.file_build_name}\">${before_file.file_build_name}</a>`,
        after_link: `<a href=\"${after_file.file_build_name}\">${after_file.file_build_name}</a>`
    });

    fs.writeFileSync(file_build_path, file_compiled);
}

let index_compiled = index_template({
    list_items: fileNames.map(file => `<li><a href=\"${file.file_build_name}\">${file.file_build_name}</a></li>`).join("\n")
});

fs.writeFileSync(path.join(build_dir, "index.html"), index_compiled);