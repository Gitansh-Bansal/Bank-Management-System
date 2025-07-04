const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const { spawn } = require('child_process');
const path = require('path');

const app = express();
// Use the port provided by the environment (for deployment platforms like Render, Railway, etc.)
const port = process.env.PORT || 3001;

// Middleware
app.use(cors());
app.use(bodyParser.json());

// Add this at the top, after app is created
app.use((req, res, next) => {
  console.log(`[${new Date().toISOString()}] ${req.method} ${req.url}`);
  next();
});

// Routes
app.post('/api/auth/login', (req, res) => {
  const { username, password } = req.body;
  console.log('Login attempt:', { username });
  
  if (!username || !password) {
    return res.status(400).json({ error: 'Username and password are required' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['login', username, password]);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    if (code === 0) {
      // Login successful - get user details
      const userDetailsBackend = spawn(bankAppPath, ['get-user', username]);
      
      let userOutput = '';
      let userError = '';
      
      userDetailsBackend.stdout.on('data', (data) => {
        userOutput += data.toString();
      });
      
      userDetailsBackend.stderr.on('data', (data) => {
        userError += data.toString();
      });
      
      userDetailsBackend.on('close', (userCode) => {
        if (userCode === 0) {
          try {
            const userDetails = JSON.parse(userOutput.trim());
            res.json({ 
              message: 'Login successful',
              user: userDetails
            });
          } catch (parseError) {
            // Fallback to basic user object if parsing fails
            res.json({ 
              message: 'Login successful',
              user: {
                username,
                name: username,
                id: 1
              }
            });
          }
        } else {
          // Fallback to basic user object if getting user details fails
          res.json({ 
            message: 'Login successful',
            user: {
              username,
              name: username,
              id: 1
            }
          });
        }
      });
    } else {
      res.status(401).json({ 
        error: error || 'Invalid username or password'
      });
    }
  });
});

app.post('/api/auth/register', (req, res) => {
  const { name, phone, username, password } = req.body;
  console.log('Register attempt:', { name, phone, username });
  
  // Validate input
  if (!name || !phone || !username || !password) {
    return res.status(400).json({ error: 'All fields are required' });
  }

  // Validate phone number (10 digits)
  if (!/^\d{10}$/.test(phone)) {
    return res.status(400).json({ error: 'Phone number must be 10 digits' });
  }

  // Validate username (at least 4 characters, alphanumeric and underscore only)
  if (!/^[a-zA-Z0-9_]{4,}$/.test(username)) {
    return res.status(400).json({ error: 'Username must be at least 4 characters and contain only letters, numbers, and underscores' });
  }

  // Validate password (at least 6 characters, must include uppercase, lowercase, and numbers)
  if (!/^(?=.*[a-z])(?=.*[A-Z])(?=.*\d).{6,}$/.test(password)) {
    return res.status(400).json({ error: 'Password must be at least 6 characters and include uppercase, lowercase, and numbers' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['register', name, phone, username, password]);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    if (code === 0) {
      // Registration successful
      res.json({ 
        message: 'Registration successful',
        user: {
          name,
          username,
          phone
        }
      });
    } else {
      // Registration failed
      res.status(400).json({ 
        error: error || 'Registration failed. Please try again.'
      });
    }
  });
});

app.get('/api/accounts', (req, res) => {
  const { username } = req.query;
  console.log('Get accounts for:', { username });
  
  if (!username) {
    return res.status(400).json({ error: 'Username is required' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['get-accounts', username]);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    console.log('Backend process closed with code:', code);
    console.log('Backend output:', output);
    console.log('Backend error:', error);
    
    if (code === 0) {
      try {
        // Parse the JSON output from C++ backend
        const accounts = JSON.parse(output.trim());
        console.log('Parsed accounts:', accounts);
        res.json({ accounts });
      } catch (parseError) {
        console.error('Parse error:', parseError);
        res.status(500).json({ error: 'Failed to parse accounts data' });
      }
    } else {
      res.status(400).json({ 
        error: error || 'Failed to get accounts'
      });
    }
  });
});

app.post('/api/accounts', (req, res) => {
  const { username, type, initialBalance, password } = req.body;
  console.log('Create account:', { username, type, initialBalance, password });
  
  if (!username || !type || !initialBalance || !password) {
    return res.status(400).json({ error: 'All fields are required' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['create-account', username, password, type, initialBalance.toString()]);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    if (code === 0) {
      // Extract account number from output
      const match = output.match(/Account number: (\d+)/);
      if (match) {
        const accountNumber = parseInt(match[1]);
        res.json({ 
          message: 'Account created successfully',
          accountNumber
        });
      } else {
        res.json({ message: 'Account created successfully' });
      }
    } else {
      res.status(400).json({ 
        error: error || 'Failed to create account'
      });
    }
  });
});

app.post('/api/transactions/deposit', (req, res) => {
  const { accountNumber, amount, password } = req.body;
  console.log('Deposit request received:', { accountNumber, amount });
  
  if (!accountNumber || !amount || !password) {
    console.log('Deposit validation failed: missing fields');
    return res.status(400).json({ error: 'All fields are required' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');
  console.log('Using backend path:', bankAppPath);

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['deposit', accountNumber.toString(), amount.toString(), password]);
  console.log('Backend process spawned with args:', ['deposit', accountNumber.toString(), amount.toString(), '[HIDDEN]']);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
    console.log('Backend stdout:', data.toString());
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
    console.log('Backend stderr:', data.toString());
  });

  backend.on('close', (code) => {
    console.log('Backend process closed with code:', code);
    console.log('Backend output:', output);
    console.log('Backend error:', error);
    
    if (code === 0) {
      console.log('Deposit successful');
      res.json({ message: 'Deposit successful' });
    } else {
      console.log('Deposit failed with code:', code);
      res.status(400).json({ 
        error: error || 'Deposit failed'
      });
    }
  });
});

app.post('/api/transactions/withdraw', (req, res) => {
  const { accountNumber, amount, password } = req.body;
  console.log('Withdraw request received:', { accountNumber, amount });
  
  if (!accountNumber || !amount || !password) {
    return res.status(400).json({ error: 'All fields are required' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['withdraw', accountNumber.toString(), amount.toString(), password]);
  console.log('Backend process spawned with args:', ['withdraw', accountNumber.toString(), amount.toString(), '[HIDDEN]']);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    if (code === 0) {
      res.json({ message: 'Withdrawal successful' });
    } else {
      res.status(400).json({ 
        error: error || 'Withdrawal failed'
      });
    }
  });
});

app.post('/api/transactions/transfer', (req, res) => {
  const { fromAccount, toAccount, amount, password } = req.body;
  console.log('Transfer request received:', { fromAccount, toAccount, amount });
  
  if (!fromAccount || !toAccount || !amount || !password) {
    return res.status(400).json({ error: 'All fields are required' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['transfer', fromAccount.toString(), toAccount.toString(), amount.toString(), password]);
  console.log('Backend process spawned with args:', ['transfer', fromAccount.toString(), toAccount.toString(), amount.toString(), '[HIDDEN]']);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    if (code === 0) {
      res.json({ message: 'Transfer successful' });
    } else {
      res.status(400).json({ 
        error: error || 'Transfer failed'
      });
    }
  });
});

app.get('/api/transactions/:accountNumber', (req, res) => {
  const { accountNumber } = req.params;
  console.log('Get transactions for:', { accountNumber });
  
  if (!accountNumber) {
    return res.status(400).json({ error: 'Account number is required' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['get-transactions', accountNumber]);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    if (code === 0) {
      try {
        // Parse the JSON output from C++ backend
        const transactions = JSON.parse(output.trim());
        res.json({ transactions });
      } catch (parseError) {
        res.status(500).json({ error: 'Failed to parse transactions data' });
      }
    } else {
      res.status(400).json({ 
        error: error || 'Failed to get transactions'
      });
    }
  });
});

app.get('/api/accounts/:accountNumber', (req, res) => {
  const { accountNumber } = req.params;
  console.log('Get account details for:', { accountNumber });
  
  if (!accountNumber) {
    return res.status(400).json({ error: 'Account number is required' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['get-account', accountNumber]);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    console.log('Backend process closed with code:', code);
    console.log('Backend output:', output);
    console.log('Backend error:', error);
    
    if (code === 0) {
      try {
        // Parse the JSON output from C++ backend
        const account = JSON.parse(output.trim());
        console.log('Parsed account:', account);
        res.json(account);
      } catch (parseError) {
        console.error('Parse error:', parseError);
        res.status(500).json({ error: 'Failed to parse account data' });
      }
    } else {
      res.status(400).json({ 
        error: error || 'Failed to get account details'
      });
    }
  });
});

app.delete('/api/accounts/:accountNumber', (req, res) => {
  const { accountNumber } = req.params;
  const { password } = req.body;
  console.log('Close account request received:', { accountNumber });
  
  if (!accountNumber || !password) {
    return res.status(400).json({ error: 'Account number and password are required' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['close-account', accountNumber.toString(), password]);
  console.log('Backend process spawned with args:', ['close-account', accountNumber.toString(), '[HIDDEN]']);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    if (code === 0) {
      res.json({ message: 'Account closed successfully' });
    } else {
      res.status(400).json({ 
        error: error || 'Failed to close account'
      });
    }
  });
});

app.get('/api/user/profile', (req, res) => {
  const { username } = req.query;
  console.log('Get user profile for:', { username });
  
  if (!username) {
    return res.status(400).json({ error: 'Username is required' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['get-user', username]);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    if (code === 0) {
      try {
        const userDetails = JSON.parse(output.trim());
        res.json(userDetails);
      } catch (parseError) {
        res.status(500).json({ error: 'Failed to parse user data' });
      }
    } else {
      res.status(400).json({ 
        error: error || 'Failed to get user profile'
      });
    }
  });
});

app.put('/api/user/profile', (req, res) => {
  const { username, name, phone } = req.body;
  console.log('Update user profile for:', { username, name, phone });
  
  if (!username || !name || !phone) {
    return res.status(400).json({ error: 'Username, name, and phone are required' });
  }

  // Validate phone number (10 digits)
  if (!/^\d{10}$/.test(phone)) {
    return res.status(400).json({ error: 'Phone number must be 10 digits' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['update-profile', username, name, phone]);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    if (code === 0) {
      res.json({ message: 'Profile updated successfully' });
    } else {
      res.status(400).json({ 
        error: error || 'Failed to update profile'
      });
    }
  });
});

app.put('/api/user/change-password', (req, res) => {
  const { username, currentPassword, newPassword } = req.body;
  console.log('Change password request for:', { username });
  
  if (!username || !currentPassword || !newPassword) {
    return res.status(400).json({ error: 'Username, current password, and new password are required' });
  }

  // Validate new password (at least 6 characters, must include uppercase, lowercase, and numbers)
  if (!/^(?=.*[a-z])(?=.*[A-Z])(?=.*\d).{6,}$/.test(newPassword)) {
    return res.status(400).json({ error: 'New password must be at least 6 characters and include uppercase, lowercase, and numbers' });
  }

  // Get the absolute path to the bank_app executable
  const bankAppPath = path.join(__dirname, 'bin', process.platform === 'win32' ? 'bank.exe' : 'bank');

  // Spawn the C++ backend process
  const backend = spawn(bankAppPath, ['change-password', username, currentPassword, newPassword]);

  let output = '';
  let error = '';

  backend.stdout.on('data', (data) => {
    output += data.toString();
  });

  backend.stderr.on('data', (data) => {
    error += data.toString();
  });

  backend.on('close', (code) => {
    if (code === 0) {
      res.json({ message: 'Password changed successfully' });
    } else {
      res.status(400).json({ 
        error: error || 'Failed to change password'
      });
    }
  });
});

// Start server
app.listen(port, () => {
  console.log(`API server running on port ${port}`);
}); 