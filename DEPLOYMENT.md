# 🚀 GitHub Pages Deployment Guide

This guide explains how to deploy the Super Mario WebAssembly game to GitHub Pages.

## 📋 Prerequisites

- GitHub account
- Git repository for the project
- GitHub Pages enabled for your repository

## 🔄 Automated Deployment (Recommended)

### 1. Push to GitHub
```bash
git add .
git commit -m "Add Super Mario WebAssembly game"
git push origin main
```

### 2. Enable GitHub Actions
- Go to your repository settings
- Navigate to **Actions** > **General**
- Enable **"Allow all actions and reusable workflows"**

### 3. Configure GitHub Pages
- Go to **Settings** > **Pages**
- Set **Source** to **"GitHub Actions"**
- The workflow will automatically build and deploy

### 4. Access Your Game
Your game will be available at:
```
https://yourusername.github.io/repository-name/
```

## ⚙️ Workflow Details

The `.github/workflows/deploy.yml` file:
- ✅ Sets up Emscripten environment
- ✅ Builds the WebAssembly game
- ✅ Deploys to GitHub Pages automatically
- ✅ Triggers on pushes to `main` branch

## 🌐 Project Structure for GitHub Pages

```
your-repo/
├── index.html              # Landing page
├── super_mario.html        # Game page (built)
├── super_mario.js         # JS wrapper (built)
├── super_mario.wasm       # WebAssembly binary (built)
├── .github/workflows/
│   └── deploy.yml         # Deployment workflow
└── [source files...]
```

## 🔧 Manual Deployment (Alternative)

If you prefer manual deployment:

### 1. Build Locally
```bash
./build.sh
```

### 2. Create gh-pages Branch
```bash
git checkout --orphan gh-pages
git rm -rf .
cp -r build/web/* .
cp index.html .
git add .
git commit -m "Deploy to GitHub Pages"
git push origin gh-pages
```

### 3. Configure GitHub Pages
- Set **Source** to **"Deploy from a branch"**
- Select **"gh-pages"** branch

## 🐛 Troubleshooting

### Build Fails
- Check Emscripten version in workflow
- Verify CMake configuration
- Check build logs in Actions tab

### Game Won't Load
- Verify HTTPS (required for WebAssembly)
- Check browser console for errors
- Ensure proper MIME types

### 404 Errors
- Check repository name in URLs
- Verify GitHub Pages is enabled
- Ensure files are in correct directories

## 🔗 Custom Domain (Optional)

To use a custom domain:

1. Add `CNAME` file with your domain:
```bash
echo "yourdomain.com" > CNAME
```

2. Update workflow to preserve CNAME:
```yaml
- name: Deploy to GitHub Pages
  uses: peaceiris/actions-gh-pages@v3
  with:
    github_token: ${{ secrets.GITHUB_TOKEN }}
    publish_dir: ./build/web
    cname: yourdomain.com
```

## 📊 Performance Tips

- WebAssembly files are cached by browsers
- Use CDN for better global performance
- Enable gzip compression (automatic on GitHub Pages)
- Monitor Core Web Vitals for performance

---

Your Super Mario WebAssembly game is now ready for the world! 🎮✨