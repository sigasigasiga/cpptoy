local on_lsp_attach = function(event)
    local client = vim.lsp.get_client_by_id(event.data.client_id)
    local bufnr = event.buf
    if client.server_capabilities.documentFormattingProvider then
        local lsp_format_group = vim.api.nvim_create_augroup('cpptoy/lsp/autoformat', { clear = false })
        vim.api.nvim_create_autocmd('BufWritePre', {
            callback = function() vim.lsp.buf.format() end,
            buffer = bufnr,
            group = lsp_format_group,
            desc = 'Format document on write'
        })
    end
end


vim.api.nvim_create_autocmd('LspAttach', {
    group = vim.api.nvim_create_augroup('cpptoy/lsp/server_attach', {}),
    callback = on_lsp_attach,
})
